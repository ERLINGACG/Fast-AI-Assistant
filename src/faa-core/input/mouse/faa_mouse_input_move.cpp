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

#ifdef _WIN32
void FaaMouseInput::moveMouse(const MouseMoveParams& params, FaaReturnMessage& msg) const {

    int steps = std::max(2, params.steps); // 最小移动步数为 2
    int targetX = static_cast<int>(std::lround(static_cast<double>(params.x) / params.scale));
    int targetY = static_cast<int>(std::lround(static_cast<double>(params.y) / params.scale));

    const std::function moveAbs = [this, params, targetX, targetY, steps](){
        if (params.steps > 0){
            POINT start;
            GetCursorPos(&start); // 当前物理像素位置
            // 每步延时：有 durationMs 就均分，否则默认 1ms
            int delayPerStep = 1;
            if (params.durationMs > 0) {
                delayPerStep = std::max(1,static_cast<int>(params.durationMs / steps));
            }
            for (int i = 1; i <= steps; ++i) {
                if (i == steps) {
                    SetCursorPos(targetX, targetY); // 最后一步精确落点
                    break;
                }

                double t = static_cast<double>(i) / steps;
                int curX = static_cast<int>(std::lround(start.x + (targetX - start.x) * t));
                int curY = static_cast<int>(std::lround(start.y + (targetY - start.y) * t));

                SetCursorPos(curX, curY);

                if (delayPerStep > 0) {
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(delayPerStep));
                }
            }
        }
        else {
            SetCursorPos(
                static_cast<long>(static_cast<double>(params.x) / params.scale),
                 static_cast<long>(static_cast<double>(params.y) / params.scale)
            );
        }
    };

    const std::function moveRel = [this, params, steps,&msg](){
        if (!this->isInit()) {
            msg.error = "target input not init";
            return;
        }
        POINT startScreen, endScreen;
        GetCursorPos(&startScreen);                    // 已是屏幕坐标
        endScreen = {
            static_cast<long>(params.x / params.scale),
            static_cast<long>(params.y / params.scale)
        };
        ClientToScreen(this->sys->ctx.hwnd, &endScreen);              // 终点转屏幕
        if (params.steps > 0) {
            int delayPerStep = 1;
            if (params.durationMs > 0) {
                delayPerStep = std::max(1, static_cast<int>(params.durationMs / steps));
            }
            for (int i = 1; i <= steps; ++i) {
                if (i == steps) {
                    SetCursorPos(endScreen.x, endScreen.y);
                    break;
                }
                double t = static_cast<double>(i) / steps;
                int curX = static_cast<int>(std::lround(startScreen.x + (endScreen.x - startScreen.x) * t));
                int curY = static_cast<int>(std::lround(startScreen.y + (endScreen.y - startScreen.y) * t));
                SetCursorPos(curX, curY);
                if (delayPerStep > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(delayPerStep));
                }
            }
        } else {

            SetCursorPos(endScreen.x, endScreen.y);
        }
    };

    switch (params.coordinateSystem) {

        case CoordinateSystem::kSCREEN:
            moveAbs();
            break;

        case CoordinateSystem::kCLIENT:
            moveRel();
            break;

        default:
            msg.error = "invalid the coordinate system mode";
            break;
    }

}
std::pair<int, int> FaaMouseInput::getMousePos(bool is_client) const
{
    POINT pt;
    GetCursorPos(&pt);
    if (is_client && this->sys->ctx.hwnd) {
        ScreenToClient(this->sys->ctx.hwnd, &pt);
    }
    return {pt.x, pt.y};
}

#endif

