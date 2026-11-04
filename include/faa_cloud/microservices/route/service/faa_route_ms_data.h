//
// Created by HP on 2026/10/24.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_ROUTE_MS_DATA_H
#define FASTAIASSISTANT_ROOT_FAA_ROUTE_MS_DATA_H
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "info/ilog/ilog.h"

namespace faa::cloud::microservices::route::service {



    struct RegisterMsInfo {

        std::string identity;
        std::string   host;
        std::uint16_t port;
        std::int64_t  timestamp;
        std::int64_t  ttl_time;
        std::string type;
        std::string status;

        nlohmann::json serialize();
        [[nodiscard]] bool equals (const RegisterMsInfo& info) const;
    };

    struct RegisterMsSet {

        std::atomic<int> idx;
        std::mutex mutex;
        std::unordered_map<std::string, RegisterMsInfo> ms_info;

        bool push  (RegisterMsInfo& info,info::log::Ilog* log);
        bool update(const RegisterMsInfo& info,info::log::Ilog* log);
        bool remove (const std::string& identity,info::log::Ilog* log);
        bool query  (const std::string& identity,RegisterMsInfo& info);


    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_ROUTE_MS_DATA_H