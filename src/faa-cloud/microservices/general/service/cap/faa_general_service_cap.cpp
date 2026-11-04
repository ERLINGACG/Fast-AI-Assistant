//
// Created by HP on 2026/10/12.
//
#include "faa_cloud/common/frame/faa_cloud_wgc_loop.h"
#include "faa_cloud/common/ocr/faa_cloud_ocr_oper.h"
#include "faa_cloud/microservices/general/service/faa_cloud_general.h"
using faa::cloud::microservices::general::FaaCloudGeneralServices;

void FaaCloudGeneralServices::addCapThread(info::log::Ilog* log) {
    log->debug("addCapThread");
    std::cout<<std::endl;
    this->addThread("capThread",

        std::thread([this,log]() {


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



                   }

                   if (!window_capture || !window_capture->open(log)) {
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

void FaaCloudGeneralServices::addWgcCaptureOper(info::log::Ilog* log)
{
    using std::chrono::system_clock;
    using net_work::FaaCloudNetworkLoad;
    if (log) log->info("add wgc capture oper");
    this->operat.push("wgc",
        [this,log]
        (FaaCloudNetworkLoad& load,sockaddr_in& addr) -> int {
                if (log) log->info("wgc capture oper to {}",load.serialize().dump());

                auto body = load.body().data;

                if (log) log->debug("wgc capture oper to target {}",body.value("target",cap.getTarget()));
                this->cap.set(body.value("target",cap.getTarget()));
                this->signals.set(this->isCapturing,true);
                auto result = true;
                const auto start = std::chrono::steady_clock::now();
                   while (result) {
                       result = getSignalMap().get(this->isCapturing);
                       if (std::chrono::steady_clock::now() - start > std::chrono::seconds(1)) {
                           break;
                       }
                       std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                auto status = "isStop";
                if (result){
                    status = "isRunning";
                }
                load.setBody({
                     {"target",this->cap.getTarget()},
                    {"status",status},
                     {"result",result}
                });
                this->network.get().sendLoad(load,addr);


                 return 0;
           }
    );
}


