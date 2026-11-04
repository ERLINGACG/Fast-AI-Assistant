//
// Created by HP on 2026/10/27.
//
#include "faa_core/input/keyboard/faa_keyboard_input.h"

using faa::core::input::keyboard::FaaKeyboardInput;
using faa::core::input::keyboard::FaaKeyboardParams;
using faa::core::input::FaaReturnMessage;

#ifdef _WIN32
#include "faa_core/input/peripherals/sys/faa_peripherals_win32_sys.h"
#elif  __linux__
#include "faa_core/input/keyboard/sys/faa_keyboard_linux_sys.h"
#endif
using faa::core::input::peripherals::sys::FaaPeripheralsSys;



void FaaKeyboardInput::active() const {

    auto hwd_hwnd = this->sys->ctx.hwnd;
    if (!hwd_hwnd || !IsWindow(hwd_hwnd)) return;

    // 1. 如果最小化，同步恢复（不用 Async，防止还没恢复完就去切前台）
    if (IsIconic(hwd_hwnd)) {
        ShowWindow(hwd_hwnd, SW_RESTORE);
    }

    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_F24; // 使用 F24 键，正常键盘没有，不会触发任何功能
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_F24;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));


    // 3. 获取线程 ID 并附加
    DWORD targetThread = GetWindowThreadProcessId(hwd_hwnd, nullptr);
    DWORD currentThread = GetCurrentThreadId();
    if (targetThread != currentThread) {
        AttachThreadInput(currentThread, targetThread, TRUE);
    }

    // 4. 强行激活三连
    SetForegroundWindow(hwd_hwnd);
    SetFocus(hwd_hwnd);
    BringWindowToTop(hwd_hwnd);


    // 6. 解除附加
    if (targetThread != currentThread) {
        AttachThreadInput(currentThread, targetThread, FALSE);
    }

    // 7. 打印结果
    std::cout << "[activate] target=" << hwd_hwnd
              << " foreground=" << GetForegroundWindow()
              << " match=" << (hwd_hwnd == GetForegroundWindow())
              << " iconic=" << IsIconic(hwd_hwnd)
              << std::endl;
}
