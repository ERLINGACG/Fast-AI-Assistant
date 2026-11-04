//
// Created by HP on 2026/10/28.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_KEYBOARD_PARAMS_BUILD_H
#define FASTAIASSISTANT_ROOT_FAA_KEYBOARD_PARAMS_BUILD_H
#include "faa_keyboard_params.h"
#include "faa_core/faa_export.h"
#include <nlohmann/json.hpp>
namespace  faa::core::input::keyboard {
    FaaKeyboardParams FAA_CPP_EXPORT json_to_faa_keyboard_params(const nlohmann::json& json);
    nlohmann::json    FAA_CPP_EXPORT faa_keyboard_params_to_json(const FaaKeyboardParams& params);
}
#endif //FASTAIASSISTANT_ROOT_FAA_KEYBOARD_PARAMS_BUILD_H
