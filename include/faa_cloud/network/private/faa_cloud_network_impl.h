//
// Created by HP on 2026/10/22.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CLOUD_NETWORK_IMPL_H
#define FASTAIASSISTANT_ROOT_FAA_CLOUD_NETWORK_IMPL_
#include <winsock2.h>
#include <nlohmann/json.hpp>

#include "info/ilog/ilog.h"
#include "faa_core/faa_export.h"
namespace faa::cloud::net_work {

    struct FAA_CPP_EXPORT FaaCloudNetworkCtxImpl {

        WSADATA wsaData{};
        SOCKET server_socket{};
        sockaddr_in server_addr{};

        bool init(const nlohmann::json& config,info::log::Ilog* log = nullptr);
        void close();


    };
}

#endif //FASTAIASSISTANT_ROOT_FAA_CLOUD_NETWORK_IMPL_H