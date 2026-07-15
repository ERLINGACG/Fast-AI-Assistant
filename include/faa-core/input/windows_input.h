//
// Created by HP on 2026/7/6.
//

#ifndef FASTSCREEN_WINDOWS_INPUT_H
#define FASTSCREEN_WINDOWS_INPUT_H
#include <cstdint>
#include <map>
#include <string>
#include <windows.h>
#include "faa-core/faa_export.h"
namespace faa::input::windows{
    struct InputKeyInfo{
        int32_t delay_time;
        const char*  key_name;
    };
    struct MouseInfo{
       int32_t mouse_x;
       int32_t mouse_y;
       int32_t enable_smooth_move = 1;
       int32_t is_client_coord = 1;
    };

    class FAA_CPP_EXPORT WindowsInput {
        HWND handle_;
        bool attached_ = false;
        std::map<std::string, WORD> key_map{
            {"A", 0x41}, {"B", 0x42}, {"C", 0x43}, {"D", 0x44},
            {"E", 0x45}, {"F", 0x46}, {"G", 0x47}, {"H", 0x48},
            {"I", 0x49}, {"J", 0x4A}, {"K", 0x4B}, {"L", 0x4C},
            {"M", 0x4D}, {"N", 0x4E}, {"O", 0x4F}, {"P", 0x50},
            {"Q", 0x51}, {"R", 0x52}, {"S", 0x53}, {"T", 0x54},
            {"U", 0x55}, {"V", 0x56}, {"W", 0x57}, {"X", 0x58},
            {"Y", 0x59}, {"Z", 0x5A},
            // 你已有的其他键
            {"ESC", 0x1B}
        };
        public:
            WindowsInput(HWND handle_):handle_(handle_){}
            ~WindowsInput()=default;
            int activity() ;
            bool attachInput();
            bool activateWindow();
            bool detachInput();
            int simulateKeyPress(const InputKeyInfo& key_info);
            int simulateMouseMoveToCenter();
            int moveMouse(const MouseInfo& mouse_info);
            int RotateMouse(double angle); // 旋转鼠标
            int clickMouse(); // 点击鼠标



    };
}
#endif //FASTSCREEN_WINDOWS_INPUT_H