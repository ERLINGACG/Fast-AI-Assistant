//
// Created by HP on 2026/8/21.
//

#ifndef FASTSCREEN_WINDOWS_INPUT_CODE_H
#define FASTSCREEN_WINDOWS_INPUT_CODE_H
namespace faa::input::windows{
    enum RunCode{
        success=0,
        failure,
        process_error,
        key_not_found,
        angle_is_negative,
    };
}
#endif //FASTSCREEN_WINDOWS_INPUT_CODE_H