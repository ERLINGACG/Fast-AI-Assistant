//
// Created by HP on 2026/7/21.
//

#ifndef FASTSCREEN_FAA_DNN_RT_INIT_H
#define FASTSCREEN_FAA_DNN_RT_INIT_H
#include "faa_service_runtime.h"
#include "faa-core/input/windows_input.h"
#include "nlohmann/json.hpp"

namespace faa::service::rt::init{
    bool faa_wgc_init(FaaWgcCapResources& resources, const std::string& target_key, const Ilog& log);
    void faa_ocr_init(FaaServiceOcrResources& resources,nlohmann::json& config);
    bool win32_input_init(std::string target_key,std::map<std::string, std::unique_ptr<input::windows::WindowsInput>>& inputs);
}
#endif //FASTSCREEN_FAA_DNN_RT_INIT_H