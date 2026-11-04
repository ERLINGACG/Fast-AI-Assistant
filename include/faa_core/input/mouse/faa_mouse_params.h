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

        CoordinateSystem coordinateSystem;
        double scale;
        double durationMs; // 移动时间，单位毫秒
        int steps;        // 移动步数
        int x;
        int y;


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

    using FaaMouseParams = std::variant<
        MouseMoveParams,
        MouseClickParams,
        MouseScrollParams>;
}
#endif //FASTAIASSISTANT_ROOT_FAA_MOUSE_PARAMS_H