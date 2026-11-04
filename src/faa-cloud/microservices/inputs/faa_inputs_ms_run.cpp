//
// Created by HP on 2026/10/26.
//
#include "faa_cloud/microservices/inputs/service/faa_inputs_ms.h"
using faa::cloud::microservices::inputs::FaaCloudInputMS;
using faa::cloud::net_work::FaaCloudNetwork;

auto service_init(){

}

int main() {
    nlohmann::json config;
    config["ip"]="127.0.0.1";
    config["port"]=8081;
    config["service_name"]="inputs";

    auto logger=info::log::Ilog([&](){
        return info::log::IlogInit{
            true,
            true,
            true,
            true,
            true
        };
    });

    auto network = std::make_unique<FaaCloudNetwork>(&logger);
    auto input_ms = FaaCloudInputMS();
    input_ms.setLog(&logger);

    try {

        input_ms.
               setConfig(config,&logger).
               setNetwork(std::move(network),&logger).
               registerDefine(&logger).
               init();

        input_ms.addAllOper();
        input_ms.addAllThread();
        input_ms.start();
        std::cout << "inputs_ms start" << std::endl;

    } catch (const std::exception& e) {
        logger.error("registerDefine failed: {}", e.what());
        return 1;
    }

    return 0;
}