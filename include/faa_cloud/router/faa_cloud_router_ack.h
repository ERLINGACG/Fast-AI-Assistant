//
// Created by HP on 2026/8/25.
//

#ifndef FASTSCREEN_FAA_CLOUD_ROUTER_ACK_H
#define FASTSCREEN_FAA_CLOUD_ROUTER_ACK_H
#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/json.hpp>
namespace faa::cloud::router {

    enum TaskStatus {
        TaskIsRunning=0,
        TaskOver     =1,
    };



    inline std::map<std::string, TaskStatus> TaskStatusMap = {
        {"is_running", TaskIsRunning},
        {"over", TaskOver},
    };


    struct FaaCloudRouterAck {
        nlohmann::json data;
        int32_t task_id;

    };
}
#endif //FASTSCREEN_FAA_CLOUD_ROUTER_ACK_H