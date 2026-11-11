//
// Created by HP on 2026/11/11.
//
#include <functional>
#include <variant>

#include "faa_core/input/mouse/faa_mouse_input.h"
#ifdef _WIN32
#define NOMINMAX
#include "faa_core/input/peripherals/sys/faa_peripherals_win32_sys.h"
#elif  __linux__
#include "faa_core/input/peripherals/sys/faa_peripherals_linux_sys.h"
#endif

using faa::core::input::peripherals::sys::FaaPeripheralsSys;
using faa::core::input::mouse::FaaMouseInput;

# ifdef  _WIN32
void FaaMouseInput::dragMouse(const MouseDragParams& params, FaaReturnMessage& msg) const {
    // TODO: 实现拖动鼠标的功能
    if (!(params.scale > 0.0) || !std::isfinite(params.scale)) {
        msg.error = "invalid scale";
        return;
    }
    auto build_to_click_key = [this](MouseButton b,bool isDown){
        switch (b) {
        case MouseButton::kLEFT:   return isDown ? MOUSEEVENTF_LEFTDOWN    : MOUSEEVENTF_LEFTUP;
        case MouseButton::kRIGHT:  return isDown ? MOUSEEVENTF_RIGHTDOWN   : MOUSEEVENTF_RIGHTUP;
        case MouseButton::kMIDDLE: return isDown ? MOUSEEVENTF_MIDDLEDOWN  : MOUSEEVENTF_MIDDLEUP;
        default: return 0;
        }
    };

    const std::function start_move  =  [this, &params, &msg]() {
        if (params.start_x < 0 || params.start_y < 0) {
            return;
        }
        this->moveMouse({
            .coordinateSystem =  params.coordinateSystem,
            .scale      = params.scale,
            .x          = params.start_x,
            .y          = params.start_y,
        },msg);
    };


    const std::function offset_move = [this, &params, &msg]() {
        if (params.offsetX == 0 && params.offsetY == 0 ) {
            return;
        }
        this->moveMouse({
            .coordinateSystem =  params.coordinateSystem,
            .scale      = params.scale,
            .durationMs = params.durationMs,
            .steps      = params.steps,
            .x          = params.start_x + params.offsetX,
            .y          = params.start_y + params.offsetY,
        },msg);
    };

    const std::function point_and_point_move = [this, &params, &msg]() {

        if (params.end_x < 0 || params.end_y < 0) {
            return false;
        }

        this->moveMouse({
            .coordinateSystem =  params.coordinateSystem,
            .scale      = params.scale,
            .durationMs = params.durationMs,
            .steps      = params.steps,
            .x          = params.end_x,
            .y          = params.end_y,
        },msg);

        return true;
    };

    start_move();

    INPUT input_down = {0};
    input_down.type = INPUT_MOUSE;
    input_down.mi.dwFlags = build_to_click_key(params.button, true);

    SendInput(1, &input_down, sizeof(INPUT));
    if (const auto rel = point_and_point_move(); !rel) {
        offset_move();
    }

    INPUT input_up = {0};
    input_up.type = INPUT_MOUSE;
    input_up.mi.dwFlags = build_to_click_key(params.button, false);
    SendInput(1, &input_up, sizeof(INPUT));



}


#endif
