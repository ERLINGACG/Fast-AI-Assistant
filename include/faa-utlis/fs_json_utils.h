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
                  data.task_type=json["task_type"];
            }
            return true;
        }

        static bool json_to_obj(core::data::EvenTStruct& data, const std::string& json_str){
            const auto json= nlohmann::json::parse(json_str);
            if (json.contains("task_type")){
                  data.target_key=json["target_key"];
                  data.task_type =json["task_type"];
                  if (json.contains("input_list")){
                        for (const auto& item : json["input_list"]){
                            core::data::EvenTStruct::input inp;
                            inp.key_name   = item.value("key_name", "");      // 缺失则空字符串
                            inp.delay_time = item.value("delay_time", 0);     // 缺失则 0
                            inp.move_x     = item.value("move_x", -1);        // 已有默认 -1，但显式指定更好
                            inp.move_y     = item.value("move_y", -1);
                            inp.is_click   = item.value("is_click", false);
                            std::cout << inp.delay_time << std::endl;
                            std::cout << inp.move_x << std::endl;
                            std::cout << inp.move_y << std::endl;
                            std::cout << inp.is_click << std::endl;
                            std::cout << inp.key_name << std::endl;
                            data.input_list.push_back(inp);
                        }
                  }
            }
            return true;
        }
}
#endif //FASTSCREEN_FS_JSON_UTILS_H