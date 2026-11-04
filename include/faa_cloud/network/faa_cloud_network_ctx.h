//
// Created by HP on 2026/8/24.
//

#ifndef FASTSCREEN_FAA_CLOUD_NETWORK_CTX_H
#define FASTSCREEN_FAA_CLOUD_NETWORK_CTX_H
#include <winsock2.h>
#include <nlohmann/json.hpp>

#include "info/ilog/ilog.h"
#include "../code/faa_cloud_code.h"
#include "faa_core/faa_export.h"

namespace faa::cloud::net_work {


    struct FaaCloudNetworkCtxImpl;

    struct FAA_CPP_EXPORT FaaCloudNetworkCtx{

        std::unique_ptr<FaaCloudNetworkCtxImpl> impl_prt;

        nlohmann::json config_;


        auto config(const nlohmann::json& config){
            config_=config;
            return code::success;
        }


        FaaCloudNetworkCtx();
        ~FaaCloudNetworkCtx();
        FaaCloudNetworkCtx(FaaCloudNetworkCtx&&) noexcept;
        FaaCloudNetworkCtx(const FaaCloudNetworkCtx&) = delete;
        FaaCloudNetworkCtx& operator=(FaaCloudNetworkCtx&&) noexcept;
        FaaCloudNetworkCtx& operator=(const FaaCloudNetworkCtx&) = delete;


        auto& impl() const{
            return *impl_prt;
        }
    };
}
#endif //FASTSCREEN_FAA_CLOUD_NETWORK_CTX_H