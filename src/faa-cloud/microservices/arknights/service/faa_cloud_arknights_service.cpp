#include "faa_cloud/microservices/arknights/service/faa_cloud_arknights_service.h"

#include <ws2tcpip.h>

#include "faa_cloud/network/faa_cloud_network_ack.h"

using faa::cloud::microservices::arknigths::FaaArknightsService;

FaaArknightsService& FaaArknightsService::setConfig(const nlohmann::json& config){
    this->config_=config;
    return *this;
}

FaaArknightsService& FaaArknightsService::setNetwork(std::unique_ptr<net_work::FaaCloudNetwork> network){
    this->network_=std::move(network);
    return *this;
}



FaaArknightsService& FaaArknightsService::initThreadPool(std::unique_ptr<core::thread_pool::FAAThreadPool> thread_pool) {
    this->thread_pool_=std::move(thread_pool);
    return *this;
}



FaaArknightsService& FaaArknightsService::startV2(info::log::Ilog& log) {

    net_work::FaaCloudNetworkLoad load_;
    load_.serialize(nlohmann::json({
         {"operation","register"},
         {"body",
            {
                {"name","arknights"}
               }
         }
    }));
    const std::string ip  = this->config_["core_service_ip"];
    const int port           = this->config_["core_service_port"].get<int>();
    log.debug("ip:{} port:{}", ip, port);

    sockaddr_in target_addr{};
    ZeroMemory(&target_addr, sizeof(target_addr));  // 必须清零
    inet_pton(AF_INET, ip.c_str(), &target_addr.sin_addr);   // 设置目标 IP
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(port);             // 目标端口

    this->network_->sendLoad(load_,target_addr);

    std::string ack_str;
    std::vector<char> ack_buf(65535);
    this->network_->getMessage(ack_buf,ack_str,target_addr);

    if (ack_str.empty()){
        log.error("the router service is unavailable");
        return *this;
    }
    nlohmann::json ack_json=nlohmann::json::parse(ack_str);

    log.info("ack_json : {}",ack_json.dump());

    this->id_str= ack_json["body"].value("define","");
    log.info("id_str : {}",this->id_str);
    return *this;
}

FaaArknightsService& FaaArknightsService::unLine(info::log::Ilog& log) {
    return *this;
}

FaaArknightsService& FaaArknightsService::loop(info::log::Ilog& log)
{
    this->loop_thread=std::thread([this,&log]() {
        this->loop_v2(log);
    });
    this->cmd_thread=std::thread([this,&log]() {
        this->loop_cmd(log);
    });

    this->loop_thread.join();
    this->cmd_thread. join();
    return *this;
}


void FaaArknightsService::loop_v2(info::log::Ilog& log)
{
    using  net_work::FaaCloudNetworkLoad;
    if (!this->isInitialized()){
        return;
    }
    sockaddr_in target_addr{};

    std::vector<char> buffer(65535);
    while (this->running.load())    {
        std::cout.flush();
        buffer.assign(buffer.size(), 0);
        std::string message;

        this->network_->getMessage(buffer,message,target_addr);
        if (message.empty()){
            continue;
        }
        log.debug("message : {}",message);

        this->thread_pool_->enqueue([this,message,target_addr,&log]() {
                 try{
                     FaaCloudNetworkLoad load;
                     load.serialize(nlohmann::json::parse(message));

                     sockaddr_in response_addr{};

                     ZeroMemory(&response_addr, sizeof(response_addr));  // 必须清零

                     if (!load.head().request_ip.empty()){
                             response_addr.sin_family = AF_INET;
                             response_addr.sin_port = htons(load.head().request_port);             // 目标端口
                             inet_pton(AF_INET, load.head().request_ip.c_str(), &response_addr.sin_addr);
                     }else{
                         response_addr=target_addr;
                     }

                     log.debug("load.head().operation : {}",load.head().operation);
                     auto op = this->operator_map_v2.find(load.head().operation);
                     if (op!=this->operator_map_v2.end()){
                          op->second(load,response_addr);
                     }else{
                         load.setBody({{"error","unknown operation"}});
                         this->network_->sendLoad(
                             load,
                              response_addr
                         );
                     }



                 }catch (std::exception& e){
                    log.error("parse message failed: {}",e.what());
                 }
                 std::cout.flush();
            }
        );

    }
}

void FaaArknightsService::loop_cmd(info::log::Ilog& log) {
    while (this->running.load()){
        std::string cmd;
        std::cin>>cmd;
        if (cmd=="/exit"){
            this->running.store(false);
            log.info("exit");
            this->network_->close();
            break;
        }
    }
}


FaaArknightsService& FaaArknightsService::initOcr(info::log::Ilog& log) {

    log.debug("initOcr det:{}",this->config_.value("det_ocr_config",nlohmann::json({})).dump());
    log.debug("initOcr rec:{}",this->config_.value("rec_ocr_config",nlohmann::json({})).dump());

    if (!this->config_.contains("det_ocr_config")){
        log.error("det_ocr_config is not found");
        return *this;
    }

    auto det=std::make_unique<
        fdnn::rt::FaaDnnExecutor<fdnn::fcv::FaaCvDnnForTensorRT,fdnn::fcv::FaaCvCtx>>(
            std::make_unique<fdnn::fcv::FaaCvDnnForTensorRT>(
                this->config_["det_ocr_config"]["path"].get<std::string>()
                )
        );
    det->initEngine().initCtx(det_ocr->ctx);

    this->det_ocr->setConfig(this->config_.value("det_ocr_config",nlohmann::json({}))).
                   setExecutor(std::move(det)).
                   setProcess (std::make_unique<fdnn::fcv::processer::ocr::FaaOcrDetProc>());

    if (!this->config_.contains("rec_ocr_config")){
        log.error("rec_ocr_config is not found");
        return *this;
    }

    auto rec=std::make_unique<
       fdnn::rt::FaaDnnExecutor<fdnn::fcv::FaaCvDnnForTensorRT,fdnn::fcv::FaaCvCtx>>(
           std::make_unique<fdnn::fcv::FaaCvDnnForTensorRT>(
               this->config_["rec_ocr_config"].value("path","")
               )
       );

    rec->initEngine().initCtx(this->rec_ocr->ctx);

    this->rec_ocr->
        setConfig(this->config_.value("rec_ocr_config",nlohmann::json({}))).
        setExecutor(std::move(rec)).
        setProcess (
            std::make_unique<fdnn::fcv::processer::ocr::FaaOcrRecProc>(
            this->config_["rec_ocr_config"].value("voc_path","")
            )
        );
    return *this;
}



FaaArknightsService& FaaArknightsService::initYoloList(info::log::Ilog& log) {
    auto array = this->config_.
            value("yolo_list_config",nlohmann::json({})).
            value("list",nlohmann::json::array());

    log.debug("initYoloList:{}",array.dump());

    int ids = 0;
    for (auto& item:array){

        log.debug("{}",item.dump(2));

        if (!item.contains("path")){
            log.warn("yolo_list_config item path is not found to {}",item.value("name",""));
            continue;
        }
        auto trt_executor=std::make_unique<
                   common::yolo::FaaCloudYolo<
                       fdnn::fcv::FaaCvDnnForTensorRT,
                       fdnn::fcv::FaaCvCtx,
                       fdnn::fcv::processer::ob::FaaYoloProc
                    >
        >();

        auto yolo_model=std::make_unique<fdnn::rt::FaaDnnExecutor<
            TensorRTBackend,TensorRTBackendCtx>
        >(
            std::make_unique<TensorRTBackend>(
                item["path"].get<std::string>()
            )
        );
        //
        yolo_model->
            initEngine().
            initCtx(trt_executor->ctx);

        auto yolo_proc=std::make_unique<fdnn::fcv::processer::ob::FaaYoloProc>(item);

        trt_executor->
            setExecutor(std::move(yolo_model)
            ).
            setProcess(std::move(yolo_proc)).
            setConfig(item);

        //
        this->yolo_map[item.value("name","yolo_"+std::to_string(ids))]=
            std::move(trt_executor);

        log.debug("yolo_list_config item {} init done",item.value("name",""));
        ids++;
    }

    return *this;
}
