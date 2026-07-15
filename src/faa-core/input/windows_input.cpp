//
// Created by HP on 2026/7/6.
//
#include "faa-core/input/windows_input.h"

#include <iostream>
#include <ostream>
// #include <WinUser.h>
// #include <processthreadsapi.h>
// #include <synchapi.h>
#include <interception/input.h>
using faa::input::windows::WindowsInput;

int WindowsInput::activity()
{
    if (this->handle_ == nullptr)
    {
        return -1;
    }
    DWORD foreThread = GetWindowThreadProcessId(this->handle_, nullptr);
    DWORD appThread = GetCurrentThreadId();
    if (foreThread != appThread)
    {
        AttachThreadInput(foreThread, appThread, TRUE);
    }
    SetForegroundWindow(this->handle_);
    SetFocus(this->handle_);
    BringWindowToTop(this->handle_);

    if (foreThread != appThread)
    {
        AttachThreadInput(foreThread, appThread, FALSE);
    }
    return 0;
}

bool WindowsInput::attachInput() {
    if (!handle_) return false;
    DWORD foreThread = GetWindowThreadProcessId(handle_, nullptr);
    DWORD appThread = GetCurrentThreadId();
    if (foreThread != appThread) {
        if (AttachThreadInput(foreThread, appThread, TRUE)) {
            attached_ = true;
            return true;
        } else {
            DWORD err = GetLastError();
            std::cerr << "AttachThreadInput failed with error: " << err << std::endl;
            // 5 = 拒绝访问，87 = 参数无效
            return false;
        }
    }
    attached_ = false;
    return true;
}

bool WindowsInput::activateWindow()  {
    if (!handle_) return false;
    SetForegroundWindow(handle_);
    SetFocus(handle_);
    BringWindowToTop(handle_);
    return true;
}

bool WindowsInput::detachInput() {
    if (attached_) {
        DWORD foreThread = GetWindowThreadProcessId(handle_, nullptr);
        DWORD appThread = GetCurrentThreadId();
        if (foreThread != appThread) {
            AttachThreadInput(foreThread, appThread, FALSE);
        }
        attached_ = false;
    }
    return true;
}

int WindowsInput::simulateKeyPress(const InputKeyInfo& key_info)  {
    // if (!attachInput()) return -1;
    std::cout<<"Key Press"<<key_info.key_name<<std::endl;
    activateWindow();  // 确保窗口在最前
    // simulateMouseMoveToCenter();
    //
    // // 随机延迟，模仿人类反应
    // Sleep(rand() % 80 + 40);
    std::string key = key_info.key_name;
    if (!key_map.contains(key)) {
        detachInput();
        return -1;
    }
    WORD vk_code = key_map[key];
    WORD scan_code = MapVirtualKey(vk_code, MAPVK_VK_TO_VSC);

    INPUT ip = {0};
    ip.type = INPUT_KEYBOARD;
    ip.ki.wVk = vk_code;
    ip.ki.wScan = scan_code;
    ip.ki.dwFlags = 0;  // 按下
    SendInput(1, &ip, sizeof(INPUT));

    Sleep(key_info.delay_time > 0 ? key_info.delay_time : (rand() % 30 + 40));

    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));

    // ---- 结束输入上下文 ----
    // detachInput();

    return 0;
}

int WindowsInput::simulateMouseMoveToCenter() {
    RECT rect;
    if (!GetWindowRect(handle_, &rect)) return -1;
    int targetX = (rect.left + rect.right) / 2;
    int targetY = (rect.top + rect.bottom) / 2;

    POINT cur;
    GetCursorPos(&cur);
    std::cout << cur.x << ' ' << cur.y << std::endl;
    // 分段移动，模拟轨迹
    int steps = 10 + rand() % 10;
    for (int i = 1; i <= steps; ++i) {
        double ratio = static_cast<double>(i) / steps;
        int x = cur.x + static_cast<int>((targetX - cur.x) * ratio);
        int y = cur.y + static_cast<int>((targetY - cur.y) * ratio);
        SetCursorPos(x, y);
        Sleep(rand() % 10 + 5);  // 随机间隔
    }
    SetCursorPos(targetX, targetY);  // 最终到位
    return 0;
}

int WindowsInput::moveMouse(const MouseInfo& mouse_info) {
    if (!handle_) return -1;

    // 1. 尝试附加线程
    if (!attachInput()) {
        // 附加失败仍继续，仅激活窗口
        SetForegroundWindow(handle_);
        Sleep(50);
    } else {
        activateWindow();
    }

    // 2. 坐标转换
    POINT pt = { mouse_info.mouse_x, mouse_info.mouse_y };
    if (mouse_info.is_client_coord) {
        if (!ClientToScreen(handle_, &pt)) {
            detachInput();
            return -1;
        }
    }

    // 3. 执行移动
    if (mouse_info.enable_smooth_move) {
        POINT cur;
        GetCursorPos(&cur);
        int steps = 10 + rand() % 10;  // 可改为 mouse_info.steps（如果扩展）
        for (int i = 1; i <= steps; ++i) {
            const double ratio = static_cast<double>(i) / steps;
            const int x = cur.x + static_cast<int>((pt.x - cur.x) * ratio);
            const int y = cur.y + static_cast<int>((pt.y - cur.y) * ratio);
            SetCursorPos(x, y);
            Sleep(rand() % 10 + 5);
        }
        SetCursorPos(pt.x, pt.y);
    } else {
        SetCursorPos(pt.x, pt.y);
    }

    detachInput();
    return 0;
}

int WindowsInput::RotateMouse(double angle) {
       return 0;
}

int WindowsInput::clickMouse() {
    this->activateWindow();

    return 0;
}


