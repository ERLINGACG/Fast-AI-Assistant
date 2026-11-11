//
// Created by HP on 2026/11/11.
//
#include "faa_core/input/keyboard/faa_keyboard_input.h"

using faa::common::ilogger::FaaIlogger;
using faa::core::input::FaaReturnMessage;
using faa::core::input::keyboard::FaaKeyboardParams;
using faa::core::input::keyboard::FaaKeyboardInputType;
using faa::core::input::keyboard::KeyBoardPushMethod;
using faa::core::input::keyboard::FaaKeyboardInput;
FaaIlogger logger{};

int main()  {


    FaaKeyboardInput keyboard_input;

    keyboard_input.setLogger(&logger);
    keyboard_input.resetTarget("明日方舟");
    keyboard_input.active();

    auto return_msg = FaaReturnMessage{};
    auto params = FaaKeyboardParams{};

    params.input_type  = FaaKeyboardInputType::kSEND;  //send 模式
    params.push_method = KeyBoardPushMethod::kSERIAL; // 串行模式
    params.keys.push_back({"ESC"});


    keyboard_input.apply(params,return_msg);


    return 0;
}