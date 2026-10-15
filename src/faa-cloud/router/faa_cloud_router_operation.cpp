//
// Created by HP on 2026/9/16.
//
#include "faa_cloud/router/faa_cloud_router.h"
#include "faa_cloud/network/faa_cloud_network.h"
#include <ws2tcpip.h>

#include "faa_cloud/network/faa_cloud_network_ack.h"
using faa::cloud::router::FaaCloudRouter;

bool FaaCloudRouter::queryMsInfoV2(net_work::FaaCloudNetworkLoad& load, sockaddr_in& addr){
    load.clearBody();
    load.setBody(this->register_mapper.getInfo());

    this->network->sendLoad(load,addr);

    return true;
}

bool FaaCloudRouter::routeMsInfoV2(net_work::FaaCloudNetworkLoad& load, sockaddr_in& addr){


    const auto msInfo       = this->register_mapper.getMsInfo(load.head().router_to);
    if(msInfo.name.empty()){
        load.clearBody();
        load.setBody({
                {"error","ms not found to "+load.head().router_to}
            }
        );
        this->network->sendLoad(load,addr);
        return false;
    }
    this->network->sendLoad(load,msInfo.addr);
    return true;
}

bool FaaCloudRouter::registerMsV2(net_work::FaaCloudNetworkLoad& load, sockaddr_in& addr){

    std::cout<<load.unserialize().dump()<<std::endl;
    auto name = load.body().data.value("name","service");
    const std::function<bool(sockaddr_in&, sockaddr_in&)> equal_addr = [](const sockaddr_in& a, const sockaddr_in& b){

        return a.sin_addr.s_addr == b.sin_addr.s_addr && a.sin_port == b.sin_port;
    };


    auto define = name+"_"+std::to_string(this->id);
    auto isService = false;
    for (auto& val : register_mapper.map | std::views::values) {
        if (equal_addr(val.addr,addr)) {

            define = val.name;
            isService = true;
        }

    }
    this->register_mapper.registerMs(this->id,define,addr);
    load.clearBody();
    load.setBody(nlohmann::json({
        {"define",define}
    }));

    this->network->sendLoad(load,addr);
    if (!isService){
        ++this->id;
    }

    return true;
}

bool FaaCloudRouter::responseV2(net_work::FaaCloudNetworkLoad& load, sockaddr_in& addr){
    std::cout<<load.unserialize().dump()<<std::endl;
    load.clearBody();

    load.addSlice(nlohmann::json({
                  {"hello","world"}
            }
        )
    );
    std::cout<<load.head_.slice_len<<std::endl;

    std::cout<<load.unserialize().dump()<<std::endl;
    this->network->sendLoad(load,addr);
    return true;
}
