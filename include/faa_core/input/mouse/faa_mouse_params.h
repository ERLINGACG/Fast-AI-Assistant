//
// Created by HP on 2026/10/29.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_MOUSE_PARAMS_H
#define FASTAIASSISTANT_ROOT_FAA_MOUSE_PARAMS_H
#include <variant>
#include <vector>

#include "faa_mouse_enum.h"

namespace faa::core::input::mouse {



    struct MouseMoveParams {

        CoordinateSystem coordinateSystem = CoordinateSystem::kCLIENT;
        double scale = 1.25;
        double durationMs = 100; // 移动时间，单位毫秒
        int steps = 10;;        // 移动步数
        int x = 0;
        int y = 0;


    };

    struct MouseClickParams {

        struct click_param {
            MouseButton button;
            int yield_time;
            int hold_time;
            int count;
        };
        ClickMode click_mode;
        std::vector<click_param> click_params;

    };

    struct MouseScrollParams {
        int step ;
        int delta;
    };

    struct MouseDragParams {

        CoordinateSystem coordinateSystem = CoordinateSystem::kCLIENT;
        MouseButton button = MouseButton::kLEFT;

        double scale       = 1.25;
        double durationMs  = 100;
        int    steps       = 5;

        int start_x     = -1;
        int start_y     = -1;

        int end_x       = -1;
        int end_y       = -1;

        int offsetX  = 0;   // 终点额外偏移（像素，客户区坐标）
        int offsetY  = 0;



    };

    using FaaMouseParams = std::variant<
        MouseMoveParams,
        MouseClickParams,
        MouseScrollParams,
        MouseDragParams
    >;
}
#endif //FASTAIASSISTANT_ROOT_FAA_MOUSE_PARAMS_H