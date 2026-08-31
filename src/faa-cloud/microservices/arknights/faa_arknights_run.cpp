//
// Created by HP on 2026/8/26.
//
#include "faa_cloud/microservices/arknights/service/faa_cloud_arknights_service.h"

using faa::cloud::microservices::arknigths::FaaArknightsService;
using faa::cloud::net_work::FaaCloudNetwork;

int main(){
    nlohmann::json config;
    config["core_service_ip"]="127.0.0.1";
    config["core_service_port"]=8080;
    config["ip"]="127.0.0.1";
    config["port"]=8081;

    config["ms_info"]=nlohmann::json{
        {"service_name","arknights"},
        {"service_type","arknights"},
        {"operation","register"},
    };
    auto log=info::log::Ilog([&](){
        return info::log::IlogInit{
            true,
            true,
            true,
            true,
            true
        };
    });
    FaaArknightsService service;
    service.setConfig(config);
    service.
        setNetwork(std::make_unique<FaaCloudNetwork>(FaaCloudNetwork(log).init(config))).
        initThreadPool(std::make_unique<faa::core::thread_pool::FAAThreadPool>());
    service.start(log);

    if (!service.isInitialized()){
        log.error("service is initialized");
        return 1;
    }
    log.info("service is initialized");
    service.loop(log);
    return 0;
}