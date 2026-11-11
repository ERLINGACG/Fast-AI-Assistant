//
// Created by HP on 2026/10/27.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_KEYBOARD_INPUT_H
#define FASTAIASSISTANT_ROOT_FAA_KEYBOARD_INPUT_H
#include <memory>

#include "faa_core/faa_export.h"
#include "faa_core/input/keyboard/faa_keyboard_params.h"
#include "faa_core/input/public/faa_return_message.h"
#include "info/ilog/ilog.h"
#include "faa_common/ilogger/faa_ilogger.h"

namespace faa::core::input::peripherals::sys
{
    struct FaaPeripheralsSys;
}

namespace faa::core::input::keyboard {



    class FAA_CPP_EXPORT FaaKeyboardInput {

        std::unique_ptr<peripherals::sys::FaaPeripheralsSys> sys;
        // info::log::Ilog* logger = nullptr;
        common::ilogger::FaaIlogger* logger = nullptr;

        public:
            FaaKeyboardInput();
            ~FaaKeyboardInput();

            FaaKeyboardInput(FaaKeyboardInput&&) noexcept;
            FaaKeyboardInput(const FaaKeyboardInput&) = delete;
            FaaKeyboardInput& operator=(FaaKeyboardInput&&) noexcept;
            FaaKeyboardInput& operator=(const FaaKeyboardInput&) = delete;

            void setLogger(common::ilogger::FaaIlogger* logger);
            void resetTarget(const std::string& hwnd_name) const;
            void active() const;
            void apply(const FaaKeyboardParams& params, FaaReturnMessage& message) const;
            [[nodiscard]] bool isInit() const;

    };

}
#endif //FASTAIASSISTANT_ROOT_FAA_KEYBOARD_INPUT_H