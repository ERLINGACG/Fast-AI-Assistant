//
// Created by HP on 2026/10/27.
//
#include <mutex>
#include <thread>

#include "faa_core/input/keyboard/faa_keyboard_enum_map.h"
#include "faa_core/input/keyboard/faa_keyboard_input.h"
#include "faa_core/input/keyboard/faa_keyboard_params_build.h"

#ifdef _WIN32
#include "faa_core/input/peripherals/sys/faa_peripherals_win32_sys.h"
#elif  __linux__
#include "faa_core/input/peripherals/sys/faa_peripherals_linux_sys.h"
#endif
using faa::core::input::keyboard::FaaKeyboardInput;
using faa::core::input::keyboard::FaaKeyboardParams;
using faa::core::input::FaaReturnMessage;
using faa::core::input::peripherals::sys::FaaPeripheralsSys;

#ifdef _WIN32
void FaaKeyboardInput::apply(const FaaKeyboardParams& params, FaaReturnMessage& message) const {

    if (logger) logger->debug("apply keyboard params: {}", faa_keyboard_params_to_json(params).dump(2));
    message.error.clear();
    if (!this->isInit()) {
        message.error = "keyboard input tag not init";
        return;
    }
    if (params.keys.empty()) {
        message.error = "no keys provided";
        return;
    }

    switch (params.push_method) {
        case KeyBoardPushMethod::kSERIAL: {
                for (const auto& k : params.keys) {
                    auto vk_code = keyMap.find(k.key);
                    if (vk_code == keyMap.end()) {
                        message.error = "invalid key: " + k.key;
                        return;
                    }
                    const WORD scan_code = MapVirtualKey(vk_code->second, MAPVK_VK_TO_VSC);
                    INPUT ip_down = {0};
                    ip_down.type       = INPUT_KEYBOARD;
                    ip_down.ki.wVk     = vk_code->second;
                    ip_down.ki.wScan   = scan_code;
                    ip_down.ki.dwFlags = 0;  // 按下
                    ip_down.ki.dwExtraInfo = 0x000C0300;

                    INPUT ip_release = {0};
                    ip_release.type       = INPUT_KEYBOARD;
                    ip_release.ki.wVk     = vk_code->second;
                    ip_release.ki.wScan   = scan_code;
                    ip_release.ki.dwFlags = KEYEVENTF_KEYUP;  // 松开
                    ip_release.ki.dwExtraInfo = 0x000C0300;


                    LPARAM lParamDown = 1 | (scan_code << 16);
                    LPARAM lParamUp   = 1 | (scan_code << 16) | (1LL << 30) | (1LL << 31);

                    for (int i = 0; i < k.count; ++i) {

                        if (params.input_type == FaaKeyboardInputType::kPOST){

                            std::this_thread::sleep_for(std::chrono::milliseconds(k.yield_time));
                            PostMessage(this->sys->ctx.hwnd, WM_KEYDOWN, vk_code->second, lParamDown);
                            std::this_thread::sleep_for(std::chrono::milliseconds(k.hold_time));
                            PostMessage(this->sys->ctx.hwnd, WM_KEYUP, vk_code->second, lParamUp);

                        }else{

                            std::this_thread::sleep_for(std::chrono::milliseconds(k.yield_time));
                            SendInput(1, &ip_down, sizeof(INPUT));
                            std::this_thread::sleep_for(std::chrono::milliseconds(k.hold_time));
                            SendInput(1, &ip_release, sizeof(INPUT));
                        }
                    }
                }
            }
            break;
        case KeyBoardPushMethod::kPARALLEL: {
                std::vector<std::thread> threads;
                threads.reserve(params.keys.size()); // 预分配内存，防止 push/emplace 时扩容

                std::mutex input_mtx;

                for (const auto& k : params.keys) {
                    auto vk_code = keyMap.find(k.key);
                    if (vk_code == keyMap.end()) {
                        message.error = "invalid key: " + k.key;
                        for (auto& t : threads) { if (t.joinable()) t.join(); }
                        return;
                    }

                       const WORD vk = vk_code->second;
                       const WORD scan_code = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
                       auto  input_type = params.input_type;
                        // ✅ 使用 emplace_back 就地构造线程
                       threads.emplace_back([vk, scan_code, input_type, k, &input_mtx, this]() {
                            // ✅ 在内部创建 INPUT，避免 const_cast
                            INPUT ip_down = {0};
                            ip_down.type           = INPUT_KEYBOARD;
                            ip_down.ki.wVk         = vk;
                            ip_down.ki.wScan       = scan_code;
                            ip_down.ki.dwFlags     = 0;
                            ip_down.ki.dwExtraInfo = 0x000C0300;

                            INPUT ip_release = {0};
                            ip_release.type           = INPUT_KEYBOARD;
                            ip_release.ki.wVk         = vk;
                            ip_release.ki.wScan       = scan_code;
                            ip_release.ki.dwFlags     = KEYEVENTF_KEYUP;
                            ip_release.ki.dwExtraInfo = 0x000C0300;
                            LPARAM lParamDown = 1 | (scan_code << 16);
                            LPARAM lParamUp   = 1 | (scan_code << 16) | (1LL << 30) | (1LL << 31);

                            // ✅ 必须在子线程内捕获异常，防止直接闪退
                            try {
                                for (int i = 0; i < k.count; ++i) {

                                    std::this_thread::sleep_for(std::chrono::milliseconds(k.yield_time));
                                    {
                                        std::lock_guard lock(input_mtx);
                                        if (input_type == FaaKeyboardInputType::kPOST){
                                            PostMessage(this->sys->ctx.hwnd, WM_KEYDOWN, vk, lParamDown);
                                        }else{
                                            SendInput(1, &ip_down, sizeof(INPUT));
                                        }

                                    }

                                    if (k.hold_time > 0) {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(k.hold_time));
                                    }

                                    {
                                        std::lock_guard lock(input_mtx);
                                        if (input_type == FaaKeyboardInputType::kPOST){
                                            PostMessage(this->sys->ctx.hwnd, WM_KEYUP, vk, lParamUp);
                                        }else{
                                            SendInput(1, &ip_release, sizeof(INPUT));
                                        }
                                    }

                                }
                            } catch (const std::exception& e) {

                            }
                        });
                    }

                    // ✅ 统一等待所有线程结束（无论上面是正常走完还是报错退出，最后都必须 join）
                    for (auto& t : threads) {
                        if (t.joinable()) {
                            t.join();
                        }
                    }
                }
        break;
    }

}
#endif // _WIN32

#ifdef __linux__
void FaaKeyboardInput::apply(const FaaKeyboardParams& params, FaaReturnMessage& message) const {}
#endif

bool FaaKeyboardInput::isInit() const {
    return this->sys->ctx.hwnd != nullptr;
}
