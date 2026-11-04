//
// Created by HP on 2026/10/27.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_INPUT_PARAM_H
#define FASTAIASSISTANT_ROOT_FAA_INPUT_PARAM_H
#include <variant>
#include "faa_core/input/keyboard/faa_keyboard_params.h"

namespace faa::core::input {

    using FaaInputParams = std::variant<keyboard::FaaKeyboardParams>;

}
#endif //FASTAIASSISTANT_ROOT_FAA_INPUT_PARAM_H