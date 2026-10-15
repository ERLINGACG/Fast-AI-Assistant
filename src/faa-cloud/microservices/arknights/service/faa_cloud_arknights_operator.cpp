//
// Created by HP on 2026/8/31.
//
#include "faa_cloud/common/frame/faa_cloud_wgc_loop.h"
#include "faa_cloud/microservices/arknights/service/faa_cloud_arknights_service.h"
#include "faa_cloud/network/faa_cloud_network_ack.h"
#include "faa_cloud/common/yolo/faa_cloud_yolo_oper.h"
#include <stdfloat>
using faa::cloud::microservices::arknigths::FaaArknightsService;




FaaArknightsService& FaaArknightsService::setOperatorV2(info::log::Ilog& log){

    addResponseOperator(log);
    addWgcOperatorV2(log);
    addOcrOperatorV2(log);
    addWin32InputOperatorV2(log);
    addOcrAsyncOperatorV2(log);
    addGetTask(log);
    addQueryYoloMap(log);
    // addYoloUIOperator(log);
    addYoloOperator(log);

    return  *this;
}

void FaaArknightsService::addResponseOperator(info::log::Ilog& log) {
    this->operator_map_v2["response"]=[this,&log](

        net_work::FaaCloudNetworkLoad& load,sockaddr_in& response_addr){
        log.debug("response to {}",load.unserialize().dump());
        load.setBody({{"response","im find"}});
        this->network_->sendLoad(
            load,
            response_addr
        );
    };
}

void FaaArknightsService::addYoloOperator(info::log::Ilog& log) {

    using fdnn::fcv::processer::ob::FaaYoloProc;
    using fdnn::fcv::processer::ob::FaaYoloResult;
    using fdnn::fcv::processer::ob::FaaYoloParams;

    this->operator_map_v2["yolo_det"]=[this,&log](net_work::FaaCloudNetworkLoad& load,sockaddr_in& response_addr) {

        log.debug("yolo_det to {}",load.unserialize().dump());

        auto model = this->yolo_map.find(load.body().data.value("model",""));

        if (model==this->yolo_map.end() || !model->second){
            log.error("yolo_det model not found: {}",load.body().data.value("model",""));
            load.setBody({{"error","yolo_det model not found"}});
            this->network_->sendLoad(
                load,
                response_addr
            );
            return;
        }
        cv::Mat frame;
        this->frame_resources->peek(frame);

        FaaYoloResult result;
        fdnn::fcv::FaaCvOutputData output;

        faa::cloud::common::yolo::yoloOper(
            load.body().data,
            *model->second,
            frame,output,result
            );

        if (model->second->exit_type=="fp32"){
            delete[] static_cast<float*>(output.toHost());
        }
        else if (model->second->exit_type=="fp16"){
            delete[] static_cast<uint16_t*>(output.toHost());
        }

        load.setBody(result.serialize());
        this->network_->sendLoad(
            load,
            response_addr
        );


    };

}

void FaaArknightsService::addQueryYoloMap(info::log::Ilog& log) {
    this->operator_map_v2["query_yolo_map"]=[this,&log](net_work::FaaCloudNetworkLoad& load,sockaddr_in& response_addr){
        log.debug("query_yolo_map to {}",load.unserialize().dump());
        nlohmann::json result = nlohmann::json::array();
        for (const auto& key : this->yolo_map | std::views::keys){
            result.push_back(key);
        }
        load.setBody(result);
        this->network_->sendLoad(
            load,
            response_addr
        );
    };
}

void FaaArknightsService::addWgcOperatorV2(info::log::Ilog& log) {


    this->operator_map_v2["wgc"]=[this,&log](net_work::FaaCloudNetworkLoad& load,sockaddr_in& response_addr){

        if (this->get_frames_running.load()){
            load.clearBody();
            load.setBody({
                {"wgc","running"}
            });
            this->network_->sendLoad(
                load,
                response_addr
            );
            return;
        }

        this->get_frames_running.store(false);
        this->get_frames_thread=std::thread([this,&log,response_addr,load = std::move(load)]() mutable {
            const auto d3d_ctx = std::make_unique<core::cap::wgc::D3DDeviceContext>();
            const auto wgc_rt  = std::make_unique<core::cap::wgc::WGCRunTimeContext>();
            const auto window_capture =
                  std::make_unique<core::cap::wgc::FAAWindowCapture>(
                      this->config_.value("target",""),
                      log
                  );

            window_capture->
                  initD3d11(*d3d_ctx).
                  initWgcRtCtx(*wgc_rt).startSession();


           if (!window_capture->is_init()){

               log.error("wgc init failed");
               load.clearBody();
               load.setBody({
                   {"wgc","failed"}
               });
               this->network_->sendLoad(
                   load,
                   response_addr
               );
               return;
            }
            this->get_frames_running.store(true);
            load.clearBody();
            load.setBody({
                {"wgc","success"}
            });
            this->network_->sendLoad(
                load,
                response_addr
            );

            frame::wgc_loop_v2(
                this->get_frames_running,
                *window_capture,
                *this->frame_resources,
                this->config_.value("fps",60),
                this->config_.value("max_fps_rate",3)
            );
        });
        this->get_frames_thread.detach();
    };
}

void FaaArknightsService::addOcrOperatorV2(info::log::Ilog& log) {
    using load = net_work::FaaCloudNetworkLoad;
    this->operator_map_v2["ocr"]=[this,&log](load& load,sockaddr_in& response_addr)
    {
        common::ocr::FaaCloudOcrAck ack;
        auto start = std::chrono::high_resolution_clock::now();
        this->ocrOperator(log,load,ack);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> forward_time = end - start;
        log.info("ocr time: {} ms",forward_time.count());
        if (!ack.data.empty()){
            load.setBody(ack.data);
        }else{
            load.setBody({
                {"result","img is empty"}
            });
        }

        this->network_->sendLoad(load,response_addr);

    };
}

void FaaArknightsService::addWin32InputOperatorV2(info::log::Ilog& log) {
    this->operator_map_v2["win32_input"]=[this,&log](net_work::FaaCloudNetworkLoad& load, const sockaddr_in& response_addr){
        win32InputOperator(log,load.body().data);
        load.clearBody();
        load.setBody({
            {"win32_input","success"}
        });
        this->network_->sendLoad(load,response_addr);
    };
}



