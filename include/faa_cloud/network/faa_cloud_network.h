//
// Created by HP on 2026/8/24.
//

#ifndef FASTSCREEN_FAA_NETWORK_H
#define FASTSCREEN_FAA_NETWORK_H
#include <string>

#include "faa-core/faa_export.h"
#include "faa_cloud_network_ctx.h"
#include "faa_cloud_network_load.h"

namespace faa::cloud::net_work{


    class FAA_CPP_EXPORT FaaCloudNetwork {

        FaaCloudNetworkCtx ctx;
        info::log::Ilog log;
        public:
            explicit FaaCloudNetwork(const info::log::Ilog& log) : log(log){}

            FaaCloudNetwork& init(const nlohmann::json& config);
            void close();
            void getMessage (std::vector<char>& buffer,std::string& message,sockaddr_in& client_addr);
            void sendMessage(const std::string& message,const sockaddr_in& client_addr) const;
            auto& getLog(){
                return log;
            }

            void sendLoad(FaaCloudNetworkLoad& load, const sockaddr_in& client_addr) const;
            

    };
}
#endif //FASTSCREEN_FAA_NETWORK_H