//
// Created by HP on 2026/11/11.
//
#include "faa_core/input/keyboard/faa_keyboard_input.h"
#include "faa_core/input/mouse/faa_mouse_input.h"

using faa::common::ilogger::FaaIlogger;
using faa::core::input::FaaReturnMessage;
using faa::core::input::mouse::MouseMoveParams;
using faa::core::input::mouse::CoordinateSystem;
using faa::core::input::mouse::FaaMouseInput;
using faa::core::input::keyboard::FaaKeyboardInput;
using faa::core::input::mouse::MouseDragParams;
FaaIlogger logger{};


int main() {
    FaaKeyboardInput keyboard_input;

    keyboard_input.setLogger(&logger);
    keyboard_input.resetTarget("明日方舟");


    FaaMouseInput mouse_input;
    mouse_input.setLogger(&logger);
    mouse_input.resetTarget("明日方舟");
    keyboard_input.active();
    auto return_msg = FaaReturnMessage{};
    auto drag_params = MouseDragParams{};
    drag_params.scale = 1.0;
    drag_params.durationMs = 500;
    drag_params.steps = 20;
    drag_params.start_x = 732;
    drag_params.start_y = 392;

    drag_params.offsetX = -100;
    drag_params.offsetY = -100;

    mouse_input.apply(drag_params, return_msg);

    auto pos = mouse_input.getMousePos();
    logger.info("mouse pos:[ x: {} y: {} ]", pos.first, pos.second);
    auto next_start_x = pos.first;
    auto next_start_y = pos.second;

    drag_params.start_x = next_start_x;
    drag_params.start_y = next_start_y;

    drag_params.offsetX = 200;
    drag_params.offsetY = 0;

    mouse_input.apply(drag_params, return_msg);

    return 0;

}