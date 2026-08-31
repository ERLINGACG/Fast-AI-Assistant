//
// Created by HP on 2026/6/29.
//

// #include "faa_service/faa_service.h"
#include "faa_service/faa_service_orchestrator.h"
#include "faa_service/service_event/faa_service_event.h"
// using faa::service::FAAServiceUDP;
using faa::service::orchestrator::FaaServiceOrchestrator;
using faa::service::net_work::FaaServiceUDPNetwork;
int main(int argc, char** argv) {
    // std::cout << "hello" << std::endl;
    SetConsoleOutputCP(CP_UTF8);
    auto service=faa::service::event::FaaServiceEventOrchestrator(
             info::log::Ilog([&](){
                 return info::log::IlogInit{};
             })
    );
    service.
        initMetaResources("config/faa_service_config.json").
        initNetwork(std::make_unique<FaaServiceUDPNetwork>(
             FaaServiceUDPNetwork(service.getLog()
                 ).setConfig(
                     faa::service::net_work::NetWorkConfig{
                         "127.0.0.1",
                         8080,
                         1024
                     }
                 ).init()
       )).
       initThreadPools();

    service.start().join();

    return 0;
}