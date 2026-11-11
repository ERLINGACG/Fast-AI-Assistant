//
// Created by HP on 2026/10/30.
//
#include <functional>
#include <thread>
#include "faa_core/input/mouse/faa_mouse_input.h"

#ifdef _WIN32
#define NOMINMAX
#include "faa_core/input/peripherals/sys/faa_peripherals_win32_sys.h"
#elif  __linux__
#include "faa_core/input/peripherals/sys/faa_peripherals_linux_sys.h"
#endif
using faa::core::input::peripherals::sys::FaaPeripheralsSys;
using faa::core::input::mouse::FaaMouseInput;
using faa::core::input::mouse::MouseButton;
#ifdef _WIN32
void FaaMouseInput::clickMouse(const MouseClickParams& params, FaaReturnMessage& msg) const {
    if (!isInit()) {
        msg.error = "mouseInput is not init";
        return;
    }
    auto build_to_click_key = [this](MouseButton b,bool isDown){
        switch (b) {
            case MouseButton::kLEFT:   return isDown ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
            case MouseButton::kRIGHT:  return isDown ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
            case MouseButton::kMIDDLE: return isDown ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
            default: return 0;
        }
    };

    auto serial_click = [this, &params, build_to_click_key]() {
        for (const auto& [button, yield_time, hold_time, count] : params.click_params){
            INPUT input_down = {0};
            input_down.type = INPUT_MOUSE;
            input_down.mi.dwFlags = build_to_click_key(button, true);

            INPUT input_up = {0};
            input_up.type = INPUT_MOUSE;
            input_up.mi.dwFlags = build_to_click_key(button, false);
            for (int i = 0; i < count; i++) {
                if (yield_time > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(yield_time));
                }
                SendInput(1, &input_down, sizeof(INPUT));

                if (hold_time > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(hold_time));
                }
                SendInput(1, &input_up, sizeof(INPUT));
            }
        }
    };

    auto parallel_click = [this, &params, build_to_click_key]() {
        std::vector<std::thread> threads;
        for (const auto& [button, yield_time, hold_time, count] : params.click_params) {
            threads.emplace_back([this, button, yield_time, hold_time, count, build_to_click_key](){
                 INPUT input_down = {0};
                 input_down.type = INPUT_MOUSE;
                 input_down.mi.dwFlags = build_to_click_key(button, true);

                 INPUT input_up = {0};
                 input_up.type = INPUT_MOUSE;
                 input_up.mi.dwFlags = build_to_click_key(button, false);
                 for (int i = 0; i < count; i++) {
                    if (yield_time > 0) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(yield_time));
                    }
                    SendInput(1, &input_down, sizeof(INPUT));

                    if (hold_time > 0) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(hold_time));
                    }
                    SendInput(1, &input_up, sizeof(INPUT));
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    };

    switch (params.click_mode) {

        case ClickMode::kSERIAL:
            serial_click();
            break;
        case ClickMode::kPARALLEL:
            parallel_click();
            break;
    }

    // build_to_click_key(MouseButton::kLEFT);
    // if (this->logger) logger->debug("is test function to clickMouse is jump");

}


#endif
