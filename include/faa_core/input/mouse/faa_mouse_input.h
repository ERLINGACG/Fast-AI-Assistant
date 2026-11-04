//
// Created by HP on 2026/10/29.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_MOUSE_INPUT_H
#define FASTAIASSISTANT_ROOT_FAA_MOUSE_INPUT_H
#include <memory>

#include "faa_mouse_params.h"
#include "faa_core/faa_export.h"
#include "faa_core/input/public/faa_return_message.h"
#include "info/ilog/ilog.h"

namespace faa::core::input::peripherals::sys
{
    struct FaaPeripheralsSys;
}

namespace faa::core::input::mouse {

    class FAA_CPP_EXPORT FaaMouseInput {

        std::unique_ptr<peripherals::sys::FaaPeripheralsSys> sys;
        info::log::Ilog* logger = nullptr;

    public:
        FaaMouseInput();
        ~FaaMouseInput();

        FaaMouseInput(FaaMouseInput&&) noexcept;
        FaaMouseInput(const FaaMouseInput&) = delete;
        FaaMouseInput& operator=(FaaMouseInput&&) noexcept;
        FaaMouseInput& operator=(const FaaMouseInput&) = delete;

        void setLogger  (info::log::Ilog* logger) {this->logger = logger;};
        void resetTarget(const std::string& hwnd_name) const;
        void apply (const FaaMouseParams& params, FaaReturnMessage& msg) const;
        [[nodiscard]] bool isInit() const;


        void moveMouse   (const MouseMoveParams& params, FaaReturnMessage& msg) const;
        void clickMouse (const MouseClickParams& params, FaaReturnMessage& msg) const;
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_MOUSE_INPUT_H