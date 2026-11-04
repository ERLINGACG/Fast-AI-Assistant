//
// Created by HP on 2026/10/23.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CLOUD_BASE_NETWORK_OPER_H
#define FASTAIASSISTANT_ROOT_FAA_CLOUD_BASE_NETWORK_OPER_H
#include "info/ilog/ilog.h"
#include "nlohmann/json.hpp"
#include "faa_cloud/microservices/base/resource/faa_cloud_resource.h"
#include "faa_core/thread_pool/faa_thread_pool.h"

namespace faa::cloud::microservices::base::service{

    bool FAA_CPP_EXPORT registerToDefine(nlohmann::json& config,
        resource::Network& network,
        info::log::Ilog* log
    );

    void FAA_CPP_EXPORT netWorkLoop(
        resource::OperatMap<net_work::FaaCloudNetworkRequest&>& operat_map,
        const resource::Network& network,
        const std::atomic<bool>& isRunning,
        core::thread_pool::FAAThreadPool& thread_pool,
        bool isRouter = false,
        const std::function<void(net_work::FaaCloudNetworkRequest&)>& route_call = nullptr,
        info::log::Ilog* log = nullptr
    );

    void FAA_CPP_EXPORT netWorkSend(
        const resource::Network& network,
        net_work::FaaCloudNetworkRequest& request,
        const std::function<void(net_work::FaaCloudNetworkRequest&)>& call = nullptr,
        info::log::Ilog* log = nullptr
    );

    void FAA_CPP_EXPORT netWorkSendAsync(
        const resource::Network& network,
        net_work::FaaCloudNetworkRequest& request,
        const std::function<void(net_work::FaaCloudNetworkRequest&)>& call = nullptr,
        info::log::Ilog* log = nullptr
    );
}
#endif //FASTAIASSISTANT_ROOT_FAA_CLOUD_BASE_NETWORK_OPER_H