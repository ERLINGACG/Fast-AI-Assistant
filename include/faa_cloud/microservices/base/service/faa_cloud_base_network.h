//
// Created by HP on 2026/10/12.
//

#ifndef FASTSCREEN_FAA_CLOUD_BASE_NETWORK_H
#define FASTSCREEN_FAA_CLOUD_BASE_NETWORK_H
#include  "./faa_cloud_base_services.h"
namespace  faa::cloud::microservices::base {
    template<typename T>
    T& FaaCloudBaseServices<T>::registerDefine(info::log::Ilog* log) {

        auto service_name       = this->config.value("service_name","general");
        auto register_center_ip = this->config.value("core_service_ip","127.0.0.1");
        auto register_center_port  = this->config.value("core_service_port",8080);

        sockaddr_in target_addr{};
        ZeroMemory(&target_addr, sizeof(target_addr));  // 必须清零
        inet_pton(AF_INET, register_center_ip.c_str(), &target_addr.sin_addr);   // 设置目标 IP
        target_addr.sin_family = AF_INET;
        target_addr.sin_port = htons(register_center_port);             // 目标端口

        std::vector<char> load_buf(65535);

        net_work::FaaCloudNetworkLoad load;
        load.serialize(nlohmann::json({
             {"operation","register"},
             {"body",
                {
                    {"name",service_name}
                   }
             }
        }));
        this->network.get().sendLoad(load,target_addr);

        std::vector<char> ack_buf(65535);
        std::string ack_str;
        std::string define_name = "general_unknown";
        this->network.get().getMessage(ack_buf,ack_str,target_addr);
        try  {

            auto ack = nlohmann::json::parse(ack_str);
            define_name = ack["body"].value("define",define_name);



        } catch (const std::exception& e) {
            log->error("parse ack json error: {}",e.what());
        }

        this->network.identity = define_name;
        log->info("define_name : {}",define_name);
        return static_cast<T&>(*this);
    }
}
#endif //FASTSCREEN_FAA_CLOUD_BASE_NETWORK_H