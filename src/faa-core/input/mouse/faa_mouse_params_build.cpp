//
// Created by HP on 2026/10/30.
//
#include "faa_core/input/mouse/faa_mouse_params_build.h"

#include <iostream>

#include "faa_core/input/mouse/faa_mouse_enum_map.h"
using faa::core::input::mouse::CoordinateSystem;
using faa::core::input::mouse::MouseMoveParams;
using faa::core::input::mouse::MouseClickParams;
using faa::core::input::mouse::MouseScrollParams;

using faa::core::input::mouse::buttonMap;
using faa::core::input::mouse::clickMap;
using faa::core::input::mouse::coordinateSystemMap;

MouseMoveParams faa::core::input::mouse::json_to_mouse_move_params(const nlohmann::json& json) {

    MouseMoveParams params{};
    params.coordinateSystem = coordinateSystemMap[json.value("coordinateSystem",  coordinateSystemMap.begin()->first)];
    params.x = json.value("x", 0);
    params.y = json.value("y", 0);
    params.scale = json.value("scale", 1.0);
    params.steps = json.value("steps", 1);
    params.durationMs = json.value("durationMs", 0);

    return params;
}

MouseClickParams faa::core::input::mouse::json_to_mouse_click_params(
    const nlohmann::json& json)
{
    /**
     * {
     *   "clickMode":"serial",
     *   "buttons":[
     *       {
     *           "button":"",
     *           "yieldTime":0,
     *           "holdTime":0,
     *           "count":1
     *       }
     *   ]
     * }
     */
    MouseClickParams params{};
    params.click_mode = clickMap[json.value("clickMode", clickMap.begin()->first)];
    for (const auto& it : json.value("buttons", nlohmann::json::array_t())) {
        params.click_params.push_back({
            buttonMap[it.value("button", buttonMap.begin()->first)],
            it.value("yieldTime", 0),
            it.value("holdTime", 500),
            it.value("count", 1)
        });
    }
    return params;
}

MouseScrollParams faa::core::input::mouse::json_to_mouse_scroll_params(
    const nlohmann::json& json)
{
    MouseScrollParams params{};
    return params;
}

std::vector<faa::core::input::mouse::FaaMouseParams> faa::core::input::mouse::json_to_faa_mouse_params(
    const nlohmann::json& json)
{
    /**
     * {
     *   "mouse_list":[
     *       {
     *           "type":"move",
     *           "coordinateSystem":"client",
     *           "x":100,
     *           "y":100,
     *           "scale":1.0,
     *           "steps":0,
     *           "durationMs":0
     *       }
     *   ]
     * }
     */
    auto params = std::vector<FaaMouseParams>{};
    auto mouse_params_list = json.value("mouse_list", nlohmann::json::array_t());

    for (const auto& it : mouse_params_list) {
        auto type = it.value("type", moveOperMap.begin()->first);
        switch (moveOperMap[type]) {
        case MouseMoveOper::kMOVE:
            std::cout<<"MOVE"<<std::endl;
            params.emplace_back(json_to_mouse_move_params(it));
            break;
        case MouseMoveOper::kSCROLL:
            params.emplace_back(json_to_mouse_scroll_params(it));
            break;
        case MouseMoveOper::kDRAG:
            break;
        case MouseMoveOper::kCLICK:
            std::cout<<"CLICK"<<std::endl;
            params.emplace_back(json_to_mouse_click_params(it));
            break;
        }

    }
    return params;
}
