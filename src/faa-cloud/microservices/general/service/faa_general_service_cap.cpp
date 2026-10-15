//
// Created by HP on 2026/10/12.
//
#include "faa_cloud/common/frame/faa_cloud_wgc_loop.h"
#include "faa_cloud/common/ocr/faa_cloud_ocr_oper.h"
#include "faa_cloud/microservices/general/service/faa_cloud_general.h"
using faa::cloud::microservices::general::FaaCloudGeneralServices;

void FaaCloudGeneralServices::addCapThread(info::log::Ilog* log) {
    log->debug("addCapThread");
    this->addThread("capThread",

        std::thread([this,log]() {

              const core::cap::wgc::D3DDeviceContext  d3d_ctx;
              const core::cap::wgc::WGCRunTimeContext wgc_rt;
              auto window_capture = std::unique_ptr<core::cap::wgc::FAAWindowCapture>();

               auto isRunning = true;
               auto isCapturing = false;
               std::string target;

               while (isRunning) {


                   isRunning   =  this->getSignalMap().get(this->isRunning);
                   isCapturing =  this->getSignalMap().get(this->isCapturing);

                   if (target != this->cap.getTarget() ){

                       target = this->cap.getTarget();
                       log->debug("target:{}",target);

                       if (window_capture) window_capture.reset();
                       if (target.empty()) continue;

                       window_capture = std::make_unique<core::cap::wgc::FAAWindowCapture>(
                           target, *log
                       );

                       window_capture->
                            initD3d11(d3d_ctx).
                            initWgcRtCtx(wgc_rt).
                            startSession();

                   }

                   if (!window_capture || !window_capture->is_init()) {
                       target = "";
                       this->cap.set("");
                       this->getSignalMap().set(this->isCapturing,false);

                       continue;
                   }


                   frame::wgc_loop_v3([this,log,&isCapturing](){

                          return this->getSignalMap().get(this->isCapturing);

                       },
                       isCapturing,
                       *window_capture,
                       this->cap.frame_resources,
                       60,3
                       );
               }
         }
    ),

    base::resource::ThreadsMap::join,log
    );
}


