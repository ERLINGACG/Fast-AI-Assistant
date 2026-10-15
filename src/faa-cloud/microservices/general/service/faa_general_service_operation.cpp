//
// Created by HP on 2026/10/13.
//
#include "faa_cloud/microservices/general/service/faa_cloud_general.h"
using faa::cloud::microservices::general::FaaCloudGeneralServices;

void FaaCloudGeneralServices::addRequestOper(info::log::Ilog* log) {
     using net_work::FaaCloudNetworkLoad;

     if (log) log->info("add request oper");
     this->operat.push("request",
         [this,log]
         (FaaCloudNetworkLoad load,sockaddr_in& addr) -> int {
                 if (log) log->info("request oper to {}",load.unserialize().dump());
                 load.addSlice({
                     "request", "helloworld_0"
                 }
                 );
                 load.addSlice({
                     "request_0", "helloworld_1"
                 });
                 //this->network.get().sendLoad(load,addr);

                  return 0;
            }
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
                 if (log) log->info("wgc capture oper to {}",load.unserialize().dump());

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
