//
// Created by HP on 2026/10/28.
//
#include "faa_core/input/keyboard/faa_keyboard_params_build.h"
#include "faa_core/input/keyboard/faa_keyboard_enum_map.h"
using faa::core::input::keyboard::FaaKeyboardParams;
using faa::core::input::keyboard::keyMap;
using faa::core::input::keyboard::KeyBoardPushMethod;

FaaKeyboardParams faa::core::input::keyboard::json_to_faa_keyboard_params(const nlohmann::json& json)
{
    /** *
     *  {
     *      "push_method": "SERIAL",
     *      "input_type": "SEND",
     *      "keys": [
     *          {
     *          "key": "A",
     *          "count": 1,
     *          "hold_time": 500
     *          }
     *      ]
     *   }
     */
    FaaKeyboardParams params;
    params.push_method     = pushMethodMap[json.value("push_method",pushMethodMap.begin()->first)];
    params.input_type      = inputTypeMap [json.value("input_type", inputTypeMap. begin()->first)];
    for (auto& key : json.value("keys",nlohmann::json::array())) {
        params.keys.push_back({
                   .key         =key.value("key",keyMap.begin()->first),
                   .count       =key.value("count",0),
                   .yield_time  =key.value("yield_time",0),
                   .hold_time   =key.value("hold_time",500),
        });
    }

    return params;
}

nlohmann::json faa::core::input::keyboard::faa_keyboard_params_to_json(const FaaKeyboardParams& params)
{
    nlohmann::json json = nlohmann::json::object();
    std::string push_method;
    for (auto& i : pushMethodMap) {
        if (i.second == params.push_method){
            push_method = i.first;
            break;
        }
    }
    std::string input_type;
    for (auto& i : inputTypeMap) {
        if (i.second == params.input_type){
            input_type = i.first;
            break;
        }
    }
    json["push_method"] = push_method;
    json["input_type"] = input_type;
    json["keys"]        = nlohmann::json::array();
    for (const auto& [key, count, yield_time, hold_time] : params.keys) {
        auto json_obj = nlohmann::json::object();
        json_obj["key"] = key;
        json_obj["yield_time"] = yield_time;
        json_obj["count"] = count;
        json_obj["hold_time"] = hold_time;
        json["keys"].push_back(json_obj);
    }
    return json;
}
