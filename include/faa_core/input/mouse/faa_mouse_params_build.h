//
// Created by HP on 2026/10/30.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_MOUSE_PARAMS_BUILD_H
#define FASTAIASSISTANT_ROOT_FAA_MOUSE_PARAMS_BUILD_H
#include <nlohmann/json.hpp>
#include "faa_core/faa_export.h"
#include "faa_mouse_params.h"
namespace faa::core::input::mouse {

    MouseMoveParams   FAA_CPP_EXPORT json_to_mouse_move_params  (const nlohmann::json& json);
    MouseClickParams  FAA_CPP_EXPORT json_to_mouse_click_params (const nlohmann::json& json);
    MouseScrollParams FAA_CPP_EXPORT json_to_mouse_scroll_params(const nlohmann::json& json);

    std::vector<FaaMouseParams>  FAA_CPP_EXPORT json_to_faa_mouse_params(const nlohmann::json& json);
}
#endif //FASTAIASSISTANT_ROOT_FAA_MOUSE_PARAMS_BUILD_H