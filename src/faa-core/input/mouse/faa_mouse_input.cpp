//
// Created by HP on 2026/10/30.
//
#include "faa_core/input/mouse/faa_mouse_input.h"

#include <functional>
#include <map>

#include "faa_core/win_utils/faa_win_utils.h"
#ifdef _WIN32
#include "faa_core/input/peripherals/sys/faa_peripherals_win32_sys.h"
#elif  __linux__
#include "faa_core/input/peripherals/sys/faa_peripherals_linux_sys.h"
#endif
using faa::core::input::peripherals::sys::FaaPeripheralsSys;
using faa::core::input::mouse::FaaMouseInput;


FaaMouseInput::FaaMouseInput() {
    this->sys = std::make_unique<FaaPeripheralsSys>();
}
FaaMouseInput::~FaaMouseInput() = default;
FaaMouseInput::FaaMouseInput(FaaMouseInput&&) noexcept = default;
FaaMouseInput& FaaMouseInput::operator=(FaaMouseInput&&) noexcept = default;

void FaaMouseInput::resetTarget(const std::string& hwnd_name) const
{
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

void FaaMouseInput::apply(const FaaMouseParams& params, FaaReturnMessage& msg) const
{
        if (!isInit()) {
            msg.error = "mouseInput is not init";
            return;
        }
        std::visit([this,&msg]<typename T0>(T0&& arg) {                 // ① 接收 variant 的值
                using T = std::decay_t<T0>;      // ② 推导实际类型
                if constexpr (std::is_same_v<T, MouseClickParams>)
                    clickMouse(arg, msg);                   // ③ 传具体类型
                else if constexpr (std::is_same_v<T, MouseMoveParams>)
                    moveMouse(arg, msg);
        }, params);

}

bool FaaMouseInput::isInit() const{
    return this->sys->ctx.hwnd != nullptr;
}

