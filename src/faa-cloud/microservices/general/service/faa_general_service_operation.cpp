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
                 if (log) log->info("request oper to {}",load.serialize().dump());
                 load.addSlice({
                     "request", "helloworld_0"
                 }
                 );
                 load.addSlice({
                     "request_0", "helloworld_1"
                 });

                  return 0;
            }
     );
}


