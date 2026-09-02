//
// Created by HP on 2026/9/2.
//
#include "faa_cloud/common/input/faa_cloud_win32_input.h"

#include <nlohmann/json.hpp>

using faa::cloud::common::input::win32::FaaCloudWin32Input;

FaaCloudWin32Input::FaaCloudWin32Input() {
    this->operators["move_mouse"]=[this](const nlohmann::json& requires_info){
        moveMouse(requires_info);
    };
}

void FaaCloudWin32Input::moveMouse(const nlohmann::json& requires_info){

    std::lock_guard lock(mtx);
    if (!input) return;
    input->moveMouse({
        requires_info.value("x",0),
        requires_info.value("y",0),
        requires_info.value("smooth_move",false),
             requires_info.value("is_client",true),
    });
}
