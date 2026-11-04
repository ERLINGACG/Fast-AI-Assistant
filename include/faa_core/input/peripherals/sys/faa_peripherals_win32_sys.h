//
// Created by HP on 2026/10/28.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_PERIPHERALS_WIN32_SYS_H
#define FASTAIASSISTANT_ROOT_FAA_PERIPHERALS_WIN32_SYS_H
#include <string>
#include <windows.h>
namespace faa::core::input::peripherals::sys {

    struct FaaPeripheralCtx {
        HWND hwnd;
        std::string name;

    };

    struct FaaPeripheralsSys {

        FaaPeripheralCtx ctx{};

    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_PERIPHERALS_WIN32_SYS_H