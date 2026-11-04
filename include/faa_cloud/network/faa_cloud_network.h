//
// Created by HP on 2026/8/24.
//

#ifndef FASTSCREEN_FAA_NETWORK_H
#define FASTSCREEN_FAA_NETWORK_H
#include <string>

#include "faa_core/faa_export.h"
#include "faa_cloud_network_ctx.h"
#include "faa_cloud_network_load.h"
#include "faa_cloud_network_request.h"

namespace faa::cloud::net_work{


    class FAA_CPP_EXPORT FaaCloudNetwork {

        FaaCloudNetworkCtx ctx{};
        info::log::Ilog* log;
        public:
            explicit FaaCloudNetwork(info::log::Ilog* log = nullptr);

            FaaCloudNetwork& init(const nlohmann::json& config);
            void close() const;
            auto& getLog(){
                return log;
            }

            void getMessage (std::vector<char>& buffer, FaaCloudNetworkRequest& request) const;
            void sendMessage(const FaaCloudNetworkRequest& request) const;
            

    };
}
#endif //FASTSCREEN_FAA_NETWORK_H