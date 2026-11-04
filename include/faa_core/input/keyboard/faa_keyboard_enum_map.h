//
// Created by HP on 2026/10/27.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_KEYBOARD_ENUM_MAP_H
#define FASTAIASSISTANT_ROOT_FAA_KEYBOARD_ENUM_MAP_H
#include <unordered_map>

#include "faa_keyboard_input.h"

namespace faa::core::input::keyboard {

    static std::unordered_map<std::string, int> keyMap {

                {"A", 0x41}, {"B", 0x42}, {"C", 0x43}, {"D", 0x44},
                {"E", 0x45}, {"F", 0x46}, {"G", 0x47}, {"H", 0x48},
                {"I", 0x49}, {"J", 0x4A}, {"K", 0x4B}, {"L", 0x4C},
                {"M", 0x4D}, {"N", 0x4E}, {"O", 0x4F}, {"P", 0x50},
                {"Q", 0x51}, {"R", 0x52}, {"S", 0x53}, {"T", 0x54},
                {"U", 0x55}, {"V", 0x56}, {"W", 0x57}, {"X", 0x58},
                {"Y", 0x59}, {"Z", 0x5A},

                 // 数字键
                {"1", 0x31}, {"2", 0x32},
                {"3", 0x33}, {"4", 0x34},
                {"5", 0x35}, {"6", 0x36},
                {"7", 0x37}, {"8", 0x38},
                {"9", 0x39}, {"0", 0x30},

                // 你已有的其他键
                {"ESC", 0x1B},
                // 其他键
                {"SPACE", 0x20},
                {"ENTER", 0x0D},
                {"SHIFT", 0x10},

    };

    static std::unordered_map<std::string, KeyBoardPushMethod> pushMethodMap {

        {"serial",   KeyBoardPushMethod::kSERIAL},
        {"parallel", KeyBoardPushMethod::kPARALLEL},
    };

    static std::unordered_map<std::string, FaaKeyboardInputType> inputTypeMap {
        {"send", FaaKeyboardInputType::kSEND},
        {"post", FaaKeyboardInputType::kPOST},
    };


}
#endif //FASTAIASSISTANT_ROOT_FAA_KEYBOARD_ENUM_MAP_H