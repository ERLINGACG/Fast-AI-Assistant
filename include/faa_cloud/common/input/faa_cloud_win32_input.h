//
// Created by HP on 2026/9/2.
//

#ifndef FASTSCREEN_FAA_CLOUD_WIN32_INPUT_H
#define FASTSCREEN_FAA_CLOUD_WIN32_INPUT_H
#include <functional>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <utility>
#include "faa_core/faa_export.h"
#include "faa_core/input/windows_input.h"

namespace faa::cloud::common::input::win32{




    struct FAA_CPP_EXPORT  FaaCloudWin32Input{

        using input_type= std::unique_ptr<faa::input::windows::WindowsInput>;
        using operator_map = std::map<std::string,std::function<void(const nlohmann::json& requires_info)>>;

        operator_map operators;
        input_type input;
        std::mutex mtx;

        std::string target;


        FaaCloudWin32Input();

        auto set(input_type input){
            this->input=std::move(input);
        }

        auto reset(input_type input,std::string target) {
            std::lock_guard lock(mtx);
            this->input.reset();
            this->input =std::move(input);
            this->target=std::move(target);
        }


        [[nodiscard]] auto getTarget() const {
            return target;
        }

        [[nodiscard]] auto isInit() const{
            return input!=nullptr;
        }


        void moveMouse  (const nlohmann::json& requires_info);
        void sendInput  (const nlohmann::json& requires_info);
        void rotateMouse(const nlohmann::json& requires_info);
        void clickMouse (const nlohmann::json& requires_info);
        void dragMouse  (const nlohmann::json& requires_info);     // 拖动鼠标
        void yield(const nlohmann::json& requires_info);

    };


}
#endif //FASTSCREEN_FAA_CLOUD_WIN32_INPUT_H