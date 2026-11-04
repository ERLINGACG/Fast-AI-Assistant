//
// Created by HP on 2026/10/27.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_KEYBOARD_ENUM_H
#define FASTAIASSISTANT_ROOT_FAA_KEYBOARD_ENUM_H
namespace faa::core::input::keyboard {

    enum class KeyBoardPushMethod {
        kSERIAL,
        kPARALLEL,
    };

    enum class FaaKeyboardInputType{
         kSEND,
         kPOST
    };




}
#endif //FASTAIASSISTANT_ROOT_FAA_KEYBOARD_ENUM_H