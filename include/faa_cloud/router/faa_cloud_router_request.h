//
// Created by HP on 2026/8/25.
//

#ifndef FASTSCREEN_FAA_CLOUD_REQUEST_H
#define FASTSCREEN_FAA_CLOUD_REQUEST_H
#include <map>
#include <string>
#include <nlohmann/json.hpp>
namespace faa::cloud::router{

    enum TaskType{
        TaskPost             =0,
        TaskQuery            =1,
        TaskGetMicroservices =2,
        TaskRegister         =3,
    };

    inline std::map<std::string, TaskType> TaskTypeMap = {
        {"post", TaskPost},
        {"query", TaskQuery},
        {"get_microservices", TaskGetMicroservices},
    };

    struct FaaCloudRequest{

        nlohmann::json data;

        auto& unSerialization(std::string& request){
            try {
                data = nlohmann::json::parse(request);
            } catch (const nlohmann::json::exception& e) {
                data = nlohmann::json::parse("{}");
                return *this;
            }
            return *this;
        };
    };
}
#endif //FASTSCREEN_FAA_CLOUD_REQUEST_H