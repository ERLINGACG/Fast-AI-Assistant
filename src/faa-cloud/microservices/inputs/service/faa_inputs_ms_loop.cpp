//
// Created by HP on 2026/10/27.
//
#include "faa_cloud/microservices/inputs/service/faa_inputs_ms.h"
using faa::cloud::microservices::inputs::FaaCloudInputMS;
void FaaCloudInputMS::addAllThread()
{
    addNetworkLoop();
}

void FaaCloudInputMS::addNetworkLoop()
{
    if (log) log->info("addNetworkLoop");
    this->addThread("net_loop",std::thread([this](){
        std::cout << "addNetworkLoop" << std::endl;
        if (log) log->info("net_loop start");

        base::service::netWorkLoop(
            operat,
            network,
            this->signals.get(this->isRunning),
            thread_pool,
            false,
            [this](net_work::FaaCloudNetworkRequest& request){},
            log
        );
    }),
    base::resource::ThreadsMap::join,
    this->log
    );
}