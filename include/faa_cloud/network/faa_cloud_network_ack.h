//
// Created by HP on 2026/8/29.
//

#ifndef FASTSCREEN_FAA_CLOUD_ACK_H
#define FASTSCREEN_FAA_CLOUD_ACK_H
#include <nlohmann/json.hpp>

namespace faa::cloud::net_work{
    struct FaaCloudNetworkAck{
        nlohmann::json data;

        auto serialize()const{
            return data.dump();
        }
        auto unserialize(const std::string& state, const nlohmann::json& info){
            data["state"]=state;
            data["info"] = info;
        }
    };
}
#endif //FASTSCREEN_FAA_CLOUD_ACK_H