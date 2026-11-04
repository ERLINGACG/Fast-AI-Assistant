//
// Created by HP on 2026/10/27.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_KEYBOARD_PARAMS_H
#define FASTAIASSISTANT_ROOT_FAA_KEYBOARD_PARAMS_H
#include <string>
#include <vector>

#include "./faa_keyboard_enum.h"
namespace faa::core::input::keyboard {

    struct FaaKeyboardParams {

        KeyBoardPushMethod   push_method = KeyBoardPushMethod::kSERIAL;
        FaaKeyboardInputType input_type  = FaaKeyboardInputType::kSEND;

        struct child_params {
            std::string key;
            int count = 1;
            int yield_time = 0;
            int hold_time  = 500;
        };

        std::vector<child_params> keys;

    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_KEYBOARD_PARAMS_H