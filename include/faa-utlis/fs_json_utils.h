//
// Created by HP on 2026/6/29.
//

#ifndef FASTSCREEN_FS_JSON_UTILS_H
#define FASTSCREEN_FS_JSON_UTILS_H
#include <string>
#include <nlohmann/json.hpp>
namespace faa::utils::json{

        static bool json_to_obj(core::data::RequestData& data, const std::string& json_str){
            const auto json= nlohmann::json::parse(json_str);
            if (json.contains("target_key")){
                  data.target_key=json["target_key"];
            }
            return true;
        }

}
#endif //FASTSCREEN_FS_JSON_UTILS_H