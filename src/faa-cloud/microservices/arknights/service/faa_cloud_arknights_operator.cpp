//
// Created by HP on 2026/8/31.
//
#include "faa_cloud/common/frame/faa_cloud_wgc_loop.h"
#include "faa_cloud/microservices/arknights/service/faa_cloud_arknights_service.h"
using faa::cloud::microservices::arknigths::FaaArknightsService;


FaaArknightsService& FaaArknightsService::setOperator(info::log::Ilog& log){
    auto get_to_frame_for_wgc=[this,&log](){
        if (this->get_frames_running.load()){
            return;
        }
        this->get_frames_running.store(true);
        this->get_frames_thread=std::thread([this, log](){
            frame::wgc_loop(this-get_frames_running,
                this->config_.value("target","") ,
                *this->frame_resources,
                log,
                this->config_.value("fps",60),
                this->config_.value("max_fps_rate",3)
            );
        });
        this->get_frames_thread.join();

    };
    this->operator_map["get_frame_wgc"]=get_to_frame_for_wgc;

    return *this;
}
