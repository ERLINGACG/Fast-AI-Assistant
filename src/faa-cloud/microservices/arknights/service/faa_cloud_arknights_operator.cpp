//
// Created by HP on 2026/8/31.
//
#include "faa_cloud/common/frame/faa_cloud_wgc_loop.h"
#include "faa_cloud/microservices/arknights/service/faa_cloud_arknights_service.h"
#include "faa_cloud/network/faa_cloud_network_ack.h"
using faa::cloud::microservices::arknigths::FaaArknightsService;


FaaArknightsService& FaaArknightsService::setOperator(info::log::Ilog& log){
    addWgcOperator(log);
    addSleepOperator(log);
    addWin32InputOperator(log);
    return *this;
}

void FaaArknightsService::addWgcOperator(info::log::Ilog& log) {

    this->operator_map["wgc"]=[this,&log](nlohmann::json& requires_info,sockaddr_in& response_addr){
        using netWorkAck = net_work::FaaCloudNetworkAck;
        if (this->get_frames_running.load()){
            this->network_->sendMessage(
                netWorkAck().
                unserialize("success",
                    nlohmann::json(
                        {
                               {"wgc","running"}
                            }
                )
                ).serialize(),
                response_addr
            );
            return;
        }
        this->get_frames_running.store(false);
        this->get_frames_thread=std::thread([this,&log,response_addr](){

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

           netWorkAck ack;
           if (!window_capture->is_init()){

               log.error("wgc init failed");
               this->network_->sendMessage(
                   ack.unserialize("success",
                       nlohmann::json(
                           {
                                 {"wgc","failed"}
                               }
                            )
                            ).serialize(),
                    response_addr
                   );
                  return;
            }
            this->get_frames_running.store(true);
            ack.unserialize("success",nlohmann::json({
                      {"wgc","success"}
                  })
             );

            this->network_->sendMessage(ack.serialize(),response_addr);
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

void FaaArknightsService::addSleepOperator(info::log::Ilog& log){

    this->operator_map["sleep"]=[this,&log](
            const nlohmann::json& requires_info, const sockaddr_in& response_addr
    ){

        int32_t sleep=requires_info.value("sleep",0);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));

        net_work::FaaCloudNetworkAck ack;
        ack.unserialize("success",nlohmann::json({}));
        this->network_->sendMessage(ack.serialize(),response_addr);

    };
}

void FaaArknightsService::addWin32InputOperator(info::log::Ilog& log){
    this->operator_map["win32_input"]=[this,&log](
            const nlohmann::json& requires_info, const sockaddr_in& response_addr
    ){

        win32InputOperator(log,requires_info);
        net_work::FaaCloudNetworkAck ack;
        ack.unserialize("success",nlohmann::json({}));
        this->network_->sendMessage(ack.serialize(),response_addr);
    };
}


