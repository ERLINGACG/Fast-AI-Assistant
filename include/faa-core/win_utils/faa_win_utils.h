//
// Created by HP on 2026/6/30.
//

#ifndef FASTSCREEN_FAA_WIN_UTILS_H
#define FASTSCREEN_FAA_WIN_UTILS_H
#include <string>

#include "faa-core/faa_export.h"
// #include "faa_service/faa_net_work.h"
#include <windows.h>   // 确保先包含 windows.h

namespace faa::core::win_utils{


    FAA_CPP_EXPORT HWND GetWindowHandle(const std::string& window_name);
    FAA_CPP_EXPORT HWND GetWindowHandleByClass(const std::string& class_name);
    FAA_CPP_EXPORT HWND GetWindowHandleByName(const std::string& window_name);
}
#endif //FASTSCREEN_FAA_WIN_UTILS_H