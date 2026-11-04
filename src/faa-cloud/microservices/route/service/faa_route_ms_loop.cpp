//
// Created by HP on 2026/10/24.
//
#include "faa_cloud/microservices/route/service/faa_route_ms_data.h"
#include "faa_cloud/microservices/route/service/faa_route_ms.h"
using faa::cloud::microservices::route::FaaCloudRouteMS;

void FaaCloudRouteMS::setLog(info::log::Ilog* log)
{
    this->log = log;
}

void FaaCloudRouteMS::addAllThread() {
    addNetWorkLoopThread();
}



void FaaCloudRouteMS::addNetWorkLoopThread() {

    if (log) log->info("addNetWorkLoopThread");
    this->addThread("net_loop",std::thread([this](){
        if (log) log->info("net_loop start");

        base::service::netWorkLoop(
            operat,
            network,
            this->signals.get(this->isRunning),
            thread_pool,
            true,
            [this](net_work::FaaCloudNetworkRequest& request){
                if (
                    const auto oper = this->operat.map.find("route");
                    oper != this->operat.map.end()
                ) {
                    oper->second(request);
                }

            },
            log
        );
    }),
    base::resource::ThreadsMap::join,
    this->log
    );

}


