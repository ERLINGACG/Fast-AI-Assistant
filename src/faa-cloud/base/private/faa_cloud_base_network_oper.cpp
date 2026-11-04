//
// Created by HP on 2026/10/23.
//
#include "faa_cloud/microservices/base/service/private/faa_cloud_base_network_oper.h"

bool faa::cloud::microservices::base::service::registerToDefine(nlohmann::json& config,
        resource::Network& network, info::log::Ilog* log

) {

    if (log) log->debug("register to define to config {}", config.dump());
    auto service_name       = config.value("service_name","general");
    auto register_center_ip = config.value("core_service_ip","127.0.0.1");
    auto register_center_port  = config.value("core_service_port",8080);


    auto requester = net_work::FaaCloudNetworkRequest{};
    requester.resetNetwork(register_center_ip, register_center_port);

    auto requester_data = nlohmann::json::object();
    requester_data["operation"] = "register";
    requester_data["body"] = {{"this_name", service_name}};

    requester.deserialize(requester_data);

    network.network->sendMessage(requester);

    try {
        std::vector<char> buffer(65535);
        network.network->getMessage(buffer, requester);
        if (log) log->debug("receive message to network {}", requester.serialize().dump());


        if (auto body = requester.serializeBody();
            body.contains("body")) {

            network.identity = body["body"].value("identity","");
            if (network.identity.empty()) {
                if (log) log->error("not identity") ;
                return false;
            }
            if (log) log->info("get identity {}", network.identity);
            return true;
        }

    } catch (const std::exception& e){
        if (log) log->error("register failed: {}", e.what());
    }
    return false;
}