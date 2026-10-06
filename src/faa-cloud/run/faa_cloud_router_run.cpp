//
// Created by HP on 2026/8/24.
//
#include "faa_cloud/network/faa_cloud_network.h"
#include "faa_cloud/router/faa_cloud_router.h"

int main(){
    using faa::cloud::net_work::FaaCloudNetwork;
    nlohmann::json config;
    config["ip"]="127.0.0.1";
    config["port"]=8080;
    auto log=info::log::Ilog([&](){
        return info::log::IlogInit{
            true,
            true,
            true,
            true,
            true
        };
    });
    faa::cloud::router::FaaCloudRouter router(
        std::make_unique<FaaCloudNetwork>(
                        FaaCloudNetwork(log).init(config)
            )
    );
    router.
        initOperatorHandlerV2().
        initThreadPool().
        start(log).
        join();

    return 0;
}
