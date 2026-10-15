//
// Created by HP on 2026/8/6.
//

#ifndef FASTSCREEN_FAA_SERVICE_INPUT_RESOURCES_H
#define FASTSCREEN_FAA_SERVICE_INPUT_RESOURCES_H
#include <memory>
#include <mutex>

#include "faa_core/input/windows_input.h"
#include "faa_core/win_utils/faa_win_utils.h"

namespace faa::service::resources::input{

    struct FaaWin32Inputs{
        using Win32Input=faa::input::windows::WindowsInput;

        std::mutex mutex;
        std::map<std::string, std::unique_ptr<Win32Input>> inputs;

        auto contains(const std::string& key){
            std::lock_guard lock(mutex);
            return inputs.contains(key);
        }

        auto push(const std::string& key){
            auto handle= core::win_utils::GetWindowHandle(key);

            if (!handle){
                handle= core::win_utils::GetWindowHandleByClass(key);
            }
            if (!handle){
                handle= core::win_utils::GetWindowHandleByName(key);
            }
            if (!handle){
                return false;
            }
            auto win32_input=std::make_unique<Win32Input>(handle);
            inputs[key]=std::move(win32_input);
            return true;
        }

        faa::input::windows::WindowsInput* peek(const std::string& key){
            std::lock_guard lock(mutex);
            if (const auto it = inputs.find(key); it != inputs.end()) {
                return it->second.get();   // 已存在，直接返回
            }
            // 不存在则尝试创建
            if (!push(key)) {
                return nullptr;
            }
            return inputs[key].get();
        }
    };

    struct FaaInputResources{
        FaaWin32Inputs win32_inputs;
    };
}
#endif //FASTSCREEN_FAA_SERVICE_INPUT_RESOURCES_H