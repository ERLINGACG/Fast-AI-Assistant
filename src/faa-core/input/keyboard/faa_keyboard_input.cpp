//
// Created by HP on 2026/10/27.
//
#include "faa_core/input/keyboard/faa_keyboard_input.h"

#include <functional>
#include <map>

#include "faa_core/win_utils/faa_win_utils.h"
#ifdef _WIN32
#include "faa_core/input/peripherals/sys/faa_peripherals_win32_sys.h"
#elif  __linux__
#include "faa_core/input/peripherals/sys/faa_peripherals_linux_sys.h"
#endif
using faa::core::input::keyboard::FaaKeyboardInput;

using faa::core::input::keyboard::FaaKeyboardParams;
using faa::core::input::FaaReturnMessage;
using faa::core::input::peripherals::sys::FaaPeripheralsSys;

FaaKeyboardInput::FaaKeyboardInput() {
    this->sys = std::make_unique<FaaPeripheralsSys>();
}

FaaKeyboardInput::~FaaKeyboardInput() =default;
FaaKeyboardInput::FaaKeyboardInput(FaaKeyboardInput&&) noexcept = default;
FaaKeyboardInput& FaaKeyboardInput::operator=(FaaKeyboardInput&&) noexcept = default;

void FaaKeyboardInput::setLogger(common::ilogger::FaaIlogger* logger){
    this->logger = logger;

}

void FaaKeyboardInput::resetTarget(const std::string& hwnd_name) const{
    std::map<
        std::string,
        std::function<HWND(const std::string&)>
    > hwnd_map{
        {"hwnd",       win_utils::GetWindowHandle},
        {"class_name", win_utils::GetWindowHandleByName},
        {"window_name",win_utils::GetWindowHandleByName},
    };
    for (auto& [key, value] : hwnd_map) {
        this->sys->ctx.hwnd = value(hwnd_name);
        if (logger) logger->debug("find to {} of {}", key, hwnd_name);
        if (this->sys->ctx.hwnd != nullptr) {
            this->sys->ctx.name = key;
            return;
        }
    }
    if (logger) logger->error("find hwnd failed: {}", hwnd_name);

}

