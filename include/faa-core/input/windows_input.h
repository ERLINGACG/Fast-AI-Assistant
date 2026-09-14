//
// Created by HP on 2026/7/6.
//

#ifndef FASTSCREEN_WINDOWS_INPUT_H
#define FASTSCREEN_WINDOWS_INPUT_H
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <windows.h>
#include "faa-core/faa_export.h"
namespace faa::input::windows{


    struct InputKeyInfo{
        bool isPost = false;
        int32_t delay_time{};
        const char*  key_name{};
    };


    struct ClickMouseInfo{

        enum ClickType{
            LeftClick=1,
            RightClick=2,
        };

        int32_t count;
        int32_t delay_time;
        std::string click_type_name;
    };

    struct MouseInfo{
           int32_t mouse_x{};
           int32_t mouse_y{};
           int32_t enable_smooth_move = 1;
           int32_t is_client_coord = 0;

    };

    struct RotateMouseInfo {

        double sensitivity{}; // 旋转灵敏度
        double angleX{};     // 水平旋转角度
        double angleY{};     // 垂直旋转角度
        int32_t step{};      // 旋转步长
        int32_t delay_time{}; // 旋转延迟时间

        [[nodiscard]] auto totalDx() const{
            return static_cast<int>(angleX * sensitivity);
        }
        [[nodiscard]] auto totalDy() const{
            return static_cast<int>(angleY * sensitivity);
        }
    };

    struct DragMouseInfo{
         std::string click_name;
         int32_t move_x;
         int32_t move_y;
         int32_t step;
         int32_t step_delay;
    };


    struct RollMouseInfo {
        int delta = 0;           // 总滚动量，单位 WHEEL_DELTA(120)，正上/负下，水平时正右/负左
        int step = 1;            // 分几步发送
        int step_delay = 10;     // 每步间隔 ms
        bool horizontal = false; // false=垂直滚轮，true=水平滚轮
    };


    class FAA_CPP_EXPORT WindowsInput {
        HWND handle_;
        bool attached_ = false;
        std::map<std::string, ClickMouseInfo::ClickType> mouse_click_map{
            {"left_click", ClickMouseInfo::ClickType::LeftClick},
            {"right_click", ClickMouseInfo::RightClick},
        };
        std::map<std::string, WORD> key_map{
            {"A", 0x41}, {"B", 0x42}, {"C", 0x43}, {"D", 0x44},
            {"E", 0x45}, {"F", 0x46}, {"G", 0x47}, {"H", 0x48},
            {"I", 0x49}, {"J", 0x4A}, {"K", 0x4B}, {"L", 0x4C},
            {"M", 0x4D}, {"N", 0x4E}, {"O", 0x4F}, {"P", 0x50},
            {"Q", 0x51}, {"R", 0x52}, {"S", 0x53}, {"T", 0x54},
            {"U", 0x55}, {"V", 0x56}, {"W", 0x57}, {"X", 0x58},
            {"Y", 0x59}, {"Z", 0x5A},

            {"F1", VK_F1}, {"F2", VK_F2}, {"F3", VK_F3}, {"F4", VK_F4},
            {"F5", VK_F5}, {"F6", VK_F6}, {"F7", VK_F7}, {"F8", VK_F8},
            {"F9", VK_F9}, {"F10", VK_F10}, {"F11", VK_F11}, {"F12", VK_F12},

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
            {"ENTER", VK_RETURN},
            {"SHIFT", VK_SHIFT},
        };
        public:
            WindowsInput(HWND handle_):handle_(handle_){}
            ~WindowsInput()=default;
            int activity() ;
            bool attachInput();
            bool activateWindow();
            bool detachInput();
            int simulateKeyPress(const InputKeyInfo& key_info);
            int simulateKeys(std::vector<InputKeyInfo>& key_infos);
            int simulateMouseMoveToCenter();
            int moveMouse(const MouseInfo& mouse_info);
            int rotateMouse(const RotateMouseInfo& rotate_info); // 旋转鼠标
            int clickMouse(const ClickMouseInfo& click_info);   // 点击鼠标
            int dragMouse(const DragMouseInfo& drag_info);     // 拖动鼠标
            int rollMouse(const RollMouseInfo& roll_info);     // 滚动鼠标


    };
}
#endif //FASTSCREEN_WINDOWS_INPUT_H