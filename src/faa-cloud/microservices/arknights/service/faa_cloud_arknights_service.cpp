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

FaaArknightsService& FaaArknightsService::start(info::log::Ilog& log){
    if (!this->config_.contains("core_service_ip") && !this->config_.contains("core_service_port")){
        log.error("core_service_ip or core_service_port not found in config");
        return *this;
    }
    if (!this->network_){
        log.error("network not found in config");
        return *this;
    }
    const std::string ip = this->config_["core_service_ip"];
    const int port          = this->config_["core_service_port"].get<int>();

    const auto service_info=this->config_.value("ms_info",nlohmann::json{});
    log.debug("ip:{} port:{}", ip, port);

    sockaddr_in target_addr{};
    ZeroMemory(&target_addr, sizeof(target_addr));  // 必须清零
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(port);             // 目标端口

    inet_pton(AF_INET, ip.c_str(), &target_addr.sin_addr);
    if (service_info.empty()){
         this->network_->sendMessage("{}",target_addr);
    }else{
         this->network_->sendMessage(service_info.dump(),target_addr);
    }

    std::string ack_str;
    std::vector<char> ack_buf(65535);
    this->network_->getMessage(ack_buf,ack_str,target_addr);

    try{
        nlohmann::json ack_json=nlohmann::json::parse(ack_str);
        log.info("ack_json : {}",ack_json.dump());

        this->id_str=ack_json["info"]["id_str"].get<std::string>();
        net_work::FaaCloudNetworkAck ack;
        nlohmann::json reg_ack;
        reg_ack["id_str"]=this->id_str;
        reg_ack["ack"]="ok";
        ack.unserialize("success",reg_ack);

        this->network_->sendMessage(ack.serialize(),target_addr);

    }catch (std::exception& e){
        log.error("parse ack_str failed: {}",e.what());
        return *this;
    }

    return *this;

}



void FaaArknightsService::loop(info::log::Ilog& log) {
    if (!this->isInitialized()){
        return;
    }
    const std::string ip = this->config_["core_service_ip"];
    const int port          = this->config_["core_service_port"].get<int>();
    sockaddr_in target_addr{};

    ZeroMemory(&target_addr, sizeof(target_addr));  // 必须清零
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(port);             // 目标端口
    inet_pton(AF_INET, ip.c_str(), &target_addr.sin_addr);

    while (this->running.load()){

         std::vector<char> msg_buf(65535);
         std::string msg_str;
         this->network_->getMessage(msg_buf,msg_str,target_addr);

         this->thread_pool_->enqueue([this,msg_buf,msg_str,target_addr, &log](){

              try {
                    nlohmann::json msg_json=nlohmann::json::parse(msg_str);
                    if (!msg_json.contains("info")){
                        return;
                    }
                    const auto response_ip      =msg_json["info"].value("requires_ip","");
                    const auto response_port       =msg_json["info"].value("requires_port",0);

                    sockaddr_in response_addr{};
                    if (!response_ip.empty()){
                        ZeroMemory(&response_addr, sizeof(response_addr));  // 必须清零
                        response_addr.sin_family = AF_INET;
                        response_addr.sin_port = htons(response_port);             // 目标端口
                        inet_pton(AF_INET, response_ip.c_str(), &response_addr.sin_addr);
                    }else{
                        response_addr=target_addr;
                    }

                   auto requires_detail=msg_json["info"].value("requires_detail",nlohmann::json());
                   log.debug("requires_detail : {}",requires_detail.dump());


                     auto operator_func=this->operator_map.find(requires_detail.value("operation",""));
                     if (operator_func!=this->operator_map.end()){
                          operator_func->second(requires_detail,response_addr);
                     }else{
                          log.error("unknown operation: {}",requires_detail.value("operation",""));
                          auto ack=net_work::FaaCloudNetworkAck();
                          ack.unserialize(
                              "unknown operation",
                               nlohmann::json(
                                  {"operation",requires_detail.value("operation","")}
                              )
                          );
                          this->network_->sendMessage(ack.serialize(),response_addr);
                     }


              }catch (std::exception& e){
                  log.error("parse msg_str failed: {}",e.what());
              }
         });
    }
}


FaaArknightsService& FaaArknightsService::initOcr(info::log::Ilog& log){
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