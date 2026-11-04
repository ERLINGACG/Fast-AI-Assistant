//
// Created by HP on 2026/10/22.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CLOUD_NETWORK_REQUEST_H
#define FASTAIASSISTANT_ROOT_FAA_CLOUD_NETWORK_REQUEST_H
#include <memory>
#include "faa_cloud_network_load.h"
#include <nlohmann/json.hpp>
#include "faa_core/faa_export.h"
namespace faa::cloud::net_work{

    struct FaaCloudNetworkRequestImpl;
    struct FAA_CPP_EXPORT FaaCloudNetworkRequest{

        std::unique_ptr<FaaCloudNetworkRequestImpl> impl_prt;
        FaaCloudNetworkLoad load{};



        FaaCloudNetworkRequest();
        ~FaaCloudNetworkRequest();
        FaaCloudNetworkRequest(FaaCloudNetworkRequest&&) noexcept;
        FaaCloudNetworkRequest(const FaaCloudNetworkRequest&) = delete;
        FaaCloudNetworkRequest& operator=(FaaCloudNetworkRequest&&) noexcept;
        FaaCloudNetworkRequest& operator=(const FaaCloudNetworkRequest&) = delete;

        [[nodiscard]] auto& impl() const {
            return impl_prt;
        }
        void resetNetwork(const std::string& ip,uint16_t port) const;

        void clear();
        void clearBody();

        void deserialize(const nlohmann::json& data);
        void deserializeHead(const nlohmann::json& data);
        void deserializeBody(const nlohmann::json& data);

        // std::pair<std::string, uint16_t> getOriginHost() const;


        [[nodiscard]] nlohmann::json serializeHead() const;
        [[nodiscard]] nlohmann::json serializeBody() const;
        [[nodiscard]] nlohmann::json serialize() const;

        void setBody(const nlohmann::json& data);
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_CLOUD_NETWORK_REQUEST_H