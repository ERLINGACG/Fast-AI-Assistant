//
// Created by HP on 2026/10/24.
//
#include "faa_cloud/microservices/route/service/faa_route_ms.h"
#include <nlohmann/json.hpp>
using faa::cloud::microservices::route::FaaCloudRouteMS;
using faa::cloud::net_work::FaaCloudNetwork;
using info::log::Ilog;
int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);         // 输入 UTF-8
    auto logger = Ilog([&]() {
        return info::log::IlogInit{
            true,
            false,
            true,
            true,
            true
        };
    });
    nlohmann::json config = nlohmann::json::object();

    auto network = std::make_unique<FaaCloudNetwork>(&logger);
    // network->init(config);

    auto route_ms = FaaCloudRouteMS();
    route_ms.setLog(&logger);
    try {
        route_ms.
            setConfig(config,&logger).
            setNetwork(std::move(network),&logger).
            init();

        route_ms.addAllThread();
        route_ms.addAllOper();
        route_ms.start();
    } catch (const std::exception& e) {
        logger.error("init failed: {}", e.what());
        return 1;
    }


        // start();

    return 0;
}
