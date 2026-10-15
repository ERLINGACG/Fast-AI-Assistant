//
// Created by HP on 2026/7/6.
//
#include "faa_core/input/windows_input.h"

#include <iostream>
#include <ostream>
// #include <WinUser.h>
// #include <processthreadsapi.h>
// #include <synchapi.h>
#include <thread>
#include <interception/input.h>

#include "faa_core/input/windows_input_code.h"
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

    std::cout<<"Key Press"<<key_info.key_name<<std::endl;
    activateWindow();  // 确保窗口在最前
    // simulateMouseMoveToCenter();
    //
    // // 随机延迟，模仿人类反应
    // Sleep(rand() % 80 + 40);
    std::string key = key_info.key_name;
    if (!key_map.contains(key)) {
        return key_not_found;
    }
    WORD vk_code = key_map[key];
    WORD scan_code = MapVirtualKey(vk_code, MAPVK_VK_TO_VSC);

    INPUT ip = {0};
    ip.type = INPUT_KEYBOARD;
    ip.ki.wVk = vk_code;
    ip.ki.wScan = scan_code;
    ip.ki.dwFlags = 0;  // 按下
    if (key_info.isPost){
        PostMessage(handle_, WM_KEYDOWN, vk_code, 0);
    }else{
         SendInput(1, &ip, sizeof(INPUT));
    }

    Sleep(key_info.delay_time > 0 ? key_info.delay_time : (rand() % 30 + 40));

    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    if (key_info.isPost){
        PostMessage(handle_, WM_KEYUP, vk_code, 0);
    }else{
        SendInput(1, &ip, sizeof(INPUT));
    }
    return success;
}

int WindowsInput::simulateKeys(std::vector<InputKeyInfo>& key_infos) {

    std::vector<std::thread> threads;
    threads.reserve(key_infos.size());

    for (auto& info : key_infos) {
        if(!key_map.contains(info.key_name)){
            return  key_not_found;
        }
        threads.emplace_back([this, &info] {
            simulateKeyPress(info);
        });
    }

    for (auto& t : threads) {
        t.join();  // 现在所有线程并行运行
    }
    return success;
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
    if (!handle_) return process_error;
    activateWindow();
    // 2. 坐标转换
    POINT pt = { mouse_info.mouse_x, mouse_info.mouse_y };
    if (mouse_info.is_client_coord) {
        if (!ClientToScreen(handle_, &pt)) {
            return process_error;
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

    // detachInput();
    return success;
}

int WindowsInput::rotateMouse(const RotateMouseInfo& rotate_info) {
    this->activateWindow();
    int totalDx = rotate_info.totalDx();
    int totalDy = rotate_info.totalDy();
    // 如果步数 <= 0 或总偏移为0，直接发送一次移动
    double stepDx = static_cast<double>(totalDx) / rotate_info.step;
    double stepDy = static_cast<double>(totalDy) / rotate_info.step;

    for (int i = 0; i < rotate_info.step; ++i) {
        // 计算本轮实际偏移（四舍五入或截断）
        int dx = static_cast<int>(std::round(stepDx));
        int dy = static_cast<int>(std::round(stepDy));
        // 为了避免由于舍入导致总偏移不足，最后一帧修正
        if (i == rotate_info.step - 1) {
            // 计算已发送的总偏移量（累加）
            // 这里简单起见，直接发送剩余的偏移（但需要跟踪累计值）
            // 更好的方法：维护累计发送值，最后补差。
            // 为了简洁，我们忽略微小误差，或采用累加方法。
        }

        INPUT input = {0};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        input.mi.dx = dx;
        input.mi.dy = dy;

        SendInput(1, &input, sizeof(INPUT));



        // 每步之间稍微延时，让平滑更自然（也可不加）
        // 延时不宜过长，否则旋转速度变慢
        Sleep(rotate_info.delay_time > 0 ? rotate_info.delay_time : (rand() % 30 + 40));  // 单位毫秒，可调整
    }
    return 0;
}

int WindowsInput::clickMouse(const ClickMouseInfo& click_info) {
    this->activateWindow();
    INPUT input = {0};
    input.type = INPUT_MOUSE;



    switch (this->mouse_click_map[click_info.click_type_name]) {
        case ClickMouseInfo::LeftClick:{

                std::cout<<"click left sleep:"<<click_info.delay_time<<std::endl;
                for (int i=0; i<click_info.count; i++){
                    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                    SendInput(1, &input, sizeof(INPUT));
                    std::this_thread::sleep_for(std::chrono::milliseconds(click_info.delay_time));

                    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                    SendInput(1, &input, sizeof(INPUT));
                    std::this_thread::sleep_for(std::chrono::milliseconds(click_info.delay_time));
                }
                break;
            }

        case ClickMouseInfo::RightClick:{
                input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
                SendInput(1, &input, sizeof(INPUT));
                std::this_thread::sleep_for(std::chrono::milliseconds(click_info.delay_time));

                input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
                SendInput(1, &input, sizeof(INPUT));
                std::this_thread::sleep_for(std::chrono::milliseconds(click_info.delay_time));
                break;
            }
        default:
            break;
        }

    return 0;
}

int WindowsInput::dragMouse(const DragMouseInfo& drag_info) {

    this->activateWindow();
    POINT curPos;
    GetCursorPos(&curPos);

    INPUT start_down = {0};
    INPUT end_down = {0};
    switch (this->mouse_click_map[drag_info.click_name]){
        case ClickMouseInfo::LeftClick:
            start_down.type = INPUT_MOUSE;
            start_down.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

            end_down.type = INPUT_MOUSE;
            end_down.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            break;
        case ClickMouseInfo::RightClick:
            start_down.type = INPUT_MOUSE;
            start_down.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

            end_down.type = INPUT_MOUSE;
            end_down.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            break;
    }
    int totalDx = drag_info.move_x ;
    int totalDy = drag_info.move_y ;
    int step = drag_info.step;

    if (step <= 0) step = 1;   // 防御
    // if (totalDx <= 0) totalDx = 0;
    // if (totalDy <= 0) totalDy = 0;

    double stepDx = static_cast<double>(totalDx) / step;
    double stepDy = static_cast<double>(totalDy) / step;

    int accumulatedDx = 0;
    int accumulatedDy = 0;


    SendInput(1, &start_down, sizeof(INPUT));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    for (int i = 0; i < step; ++i) {
        int dx = static_cast<int>(std::round(stepDx));
        int dy = static_cast<int>(std::round(stepDy));

        // 最后一帧修正累积误差
        if (i == step - 1) {
            dx = totalDx - accumulatedDx;
            dy = totalDy - accumulatedDy;
        }

        INPUT moveInput = {0};
        moveInput.type = INPUT_MOUSE;
        moveInput.mi.dwFlags = MOUSEEVENTF_MOVE;   // 相对移动
        moveInput.mi.dx = dx;
        moveInput.mi.dy = dy;
        SendInput(1, &moveInput, sizeof(INPUT));

        accumulatedDx += dx;
        accumulatedDy += dy;

        // 延迟（若 step_delay 为0则随机 5~20ms）
        // int delay = drag_info.step_delay > 0 ? drag_info.step_delay : (rand() % 15 + 5);
        std::this_thread::sleep_for(std::chrono::milliseconds(drag_info.step_delay));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    SendInput(1, &end_down, sizeof(INPUT));
    return 0;
}

int WindowsInput::rollMouse(const RollMouseInfo& roll_info){

    this->activateWindow();
    int totalDelta = roll_info.delta;
    int step = roll_info.step;

    if (step <= 0) step = 1;
    if (totalDelta == 0) return 0;

    const double stepDelta = static_cast<double>(totalDelta) / step;
    int accumulatedDelta = 0;

    for (int i = 0; i < step; ++i) {
        int delta = static_cast<int>(std::round(stepDelta));

        // 最后一帧修正累积误差
        if (i == step - 1) {
            delta = totalDelta - accumulatedDelta;
        }

        INPUT wheel = {0};
        wheel.type = INPUT_MOUSE;
        wheel.mi.dwFlags = roll_info.horizontal
                           ? MOUSEEVENTF_HWHEEL
                           : MOUSEEVENTF_WHEEL;
        wheel.mi.mouseData = static_cast<DWORD>(delta);

        SendInput(1, &wheel, sizeof(INPUT));

        accumulatedDelta += delta;

        std::this_thread::sleep_for(std::chrono::milliseconds(roll_info.step_delay));
    }
    return 0;
}


