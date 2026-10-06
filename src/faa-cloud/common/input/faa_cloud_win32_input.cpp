//
// Created by HP on 2026/9/2.
//
#include "faa_cloud/common/input/faa_cloud_win32_input.h"

#include <iostream>
#include <nlohmann/json.hpp>

using faa::cloud::common::input::win32::FaaCloudWin32Input;

FaaCloudWin32Input::FaaCloudWin32Input() {
    this->operators["move_mouse"]   =[this](const nlohmann::json& requires_info){
        moveMouse(requires_info);
    };
    this->operators["send_input"]   =[this](const nlohmann::json& requires_info){
        sendInput(requires_info);
    };
    this->operators["rotate_mouse"] =[this](const nlohmann::json& requires_info){
        rotateMouse(requires_info);
    };
    this->operators["drag_mouse"] =[this](const nlohmann::json& requires_info){
        dragMouse(requires_info);
    };
    this->operators["yield"]       =[this](const nlohmann::json& requires_info){
        yield(requires_info);
    };
    this->operators["click_mouse"] =[this](const nlohmann::json& requires_info){
        clickMouse(requires_info);
    };
}

void FaaCloudWin32Input::moveMouse(const nlohmann::json& requires_info){

    std::lock_guard lock(mtx);
    if (!input) return;
    auto dpi = requires_info.value("dpi",1.25);
    auto x   = requires_info.value("x",0) / dpi;
    auto y   = requires_info.value("y",0) / dpi;
    input->moveMouse({
        static_cast<int32_t>(x),
        static_cast<int32_t>(y),
        requires_info.value("smooth_move",false),
        requires_info.value("is_client",true),
    });
}

void FaaCloudWin32Input::sendInput(const nlohmann::json& requires_info){
    std::lock_guard lock(mtx);
    if (!input) return;
    input->simulateKeyPress({
        false,
        requires_info.value("duration",500),
        requires_info.value("key","").c_str(),
    });
}

void FaaCloudWin32Input::rotateMouse(const nlohmann::json& requires_info){
    std::lock_guard lock(mtx);
    if (!input) return;
    input->rotateMouse({
        requires_info.value("sensitivity",1.0),
        requires_info.value("angleX",0.0),
        requires_info.value("angleY",0.0),
        requires_info.value("step",10),
        requires_info.value("duration",0),
    });
}

void FaaCloudWin32Input::clickMouse(const nlohmann::json& requires_info) {
    std::lock_guard lock(mtx);
    if (!input) return;
    input->clickMouse({
                requires_info.value("count",1),
            requires_info.value("duration",500),
        requires_info.value("click_name","left_click"),
        }
    );
}

void FaaCloudWin32Input::dragMouse(const nlohmann::json& requires_info)
{
    std::lock_guard lock(mtx);
    std::cout << "drag_mouse:" << requires_info.dump() << std::endl;
    if (!input) return;

    input->dragMouse({
        requires_info.value("click_name","left_click"),
        requires_info.value("x",0),
        requires_info.value("y",0),
        requires_info.value   ("step",10),
        requires_info.value("step_delay",50),
    });
}

void FaaCloudWin32Input::yield(const nlohmann::json& requires_info){
    std::lock_guard lock(mtx);
    std::this_thread::sleep_for(std::chrono::milliseconds(requires_info.value("duration",0)));
}
