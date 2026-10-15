//
// Created by HP on 2026/7/21.
//

#ifndef FASTSCREEN_FAA_SERVICE_OPERATION_H
#define FASTSCREEN_FAA_SERVICE_OPERATION_H
#include <functional>
#include <map>
#include <string>
#include <any>
#include <opencv2/core/mat.hpp>
#include "runtime/faa_service_runtime.h"
#include <mutex>

#include "faa_core/input/windows_input.h"


namespace faa::service::operation{
    using fdnn::fcv::processer::ocr::FaaDetPreParam;
    using fdnn::fcv::processer::ocr::FaaDetPostParam;

    void wgc_run(std::mutex& lock,std::atomic<bool>& run,
                 const std::string& target,
                 std::map<std::string, std::vector<cv::Mat>>& frame_map,
                 rt::FaaWgcCapResources& wgc_cap,
                 int32_t delays=16);

    void get_img(std::mutex& lock,const std::string& target,
                 std::map<std::string, std::vector<cv::Mat>>& frame_map);

    void get_to_ocr(rt::FaaServiceOcrResources& ocr_res,
                    std::mutex& lock,
                    const std::string& target,
                    std::map<std::string, std::vector<cv::Mat>>& frame_map,
                    FaaDetPostParam param,
                    FaaSerOcrResult& result
                    );

    bool set_input_win32_key(const std::string& target,
                        std::map<std::string, std::unique_ptr<input::windows::WindowsInput>>& inputs,
                        input::windows::InputKeyInfo info
    );
    bool set_input_win32_mouse(const std::string& target,
                        std::map<std::string, std::unique_ptr<input::windows::WindowsInput>>& inputs,
                        input::windows::MouseInfo info,bool is_click=false
    );

    bool set_rotate_win32_mouse(const std::string& target,
                        std::map<std::string, std::unique_ptr<input::windows::WindowsInput>>& inputs,
                        input::windows::RotateMouseInfo info
    );







}
#endif //FASTSCREEN_FAA_SERVICE_OPERATION_H