//
// Created by HP on 2026/8/5.
//

#ifndef FASTSCREEN_FAA_SERVICE_EVENT_ACK_H
#define FASTSCREEN_FAA_SERVICE_EVENT_ACK_H
#include <string>
#include <nlohmann/json.hpp>

namespace faa::service::event{
    struct Ack{
        int32_t     code{0};
        std::string starts{};
        std::string error_info{};

        nlohmann::json detail_info{};

        auto detail(){
            detail_info["code"] = code;
            detail_info["starts"] = starts;
            detail_info["error_info"] = error_info;
            detail_info["detail"] =nlohmann::json({});
        }

        auto detail(const nlohmann::json& detail){
            detail_info["code"] = code;
            detail_info["starts"] = starts;
            detail_info["error_info"] = error_info;
            detail_info["detail"] =detail;
        }


        auto toString() const{
            return detail_info.dump(2);
        }

    };
}
#endif //FASTSCREEN_FAA_SERVICE_EVENT_ACK_H