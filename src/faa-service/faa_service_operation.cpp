//
// Created by HP on 2026/7/21.
//
#include "faa_service/faa_service_operation.h"

#include <any>
#include <mutex>
#include <opencv2/core/mat.hpp>

#include "faa_core/win_utils/faa_win_utils.h"
#include "../../include/faa_service/runtime/faa_service_runtime.h"
#include "../../include/faa_service/runtime/faa_servuce_runtime_init.h"

void faa::service::operation::wgc_run(std::mutex& lock, std::atomic<bool>& run,
                                      const std::string& target,
                                      std::map<std::string, std::vector<cv::Mat>>& frame_map,
                                      rt::FaaWgcCapResources& wgc_cap,int32_t delays){


    while (run){

        cv::Mat frame;
        core::cap::wgc::WgcTextureData wgc_data{};
        while (frame.empty()){
            wgc_cap.window_capture->getFrame(wgc_data, frame);
            std::this_thread::sleep_for(std::chrono::milliseconds(delays));
        }
        std::lock_guard guard(lock);
        if (!frame_map.contains(target)){
            frame_map[target]={};
        }
        frame_map[target].push_back(frame);
        if (frame_map[target].size()>3){
            frame_map[target].erase(frame_map[target].begin());
        }
    }
}

void faa::service::operation::get_img(std::mutex& lock,const std::string& target,
             std::map<std::string, std::vector<cv::Mat>>& frame_map)
{
    std::lock_guard guard(lock);
    auto it = frame_map.find(target);
    if (it == frame_map.end() || it->second.empty()) {
        std::cout << "get_img: frame_map is empty" << std::endl;
        return;
    }
    cv::Mat frame = it->second.back();
    if (frame.empty()) {
        std::cout << "get_img: frame is empty" << std::endl;
        return;
    }
    cv::imwrite("img.png", frame);
}

void faa::service::operation::get_to_ocr(rt::FaaServiceOcrResources& ocr_res,
                                            std::mutex& lock,
                                            const std::string& target,
                                            std::map<std::string, std::vector<cv::Mat>>& frame_map,
                                            FaaDetPostParam param,
                                            FaaSerOcrResult& result){
    if (!ocr_res.is_init){
        return;
    }



    std::lock_guard guard(lock);
    auto it = frame_map.find(target);
    if (it == frame_map.end() || it->second.empty()) {
        std::cout << "get_to_ocr: frame_map is empty" << std::endl;
        return;
    }
    cv::Mat frame = it->second.back();
    if (frame.empty()) {
        std::cout << "get_to_ocr: frame is empty" << std::endl;
        return;
    }
    auto code= fdnn::rt::code::FaaDnnRtCode();
    auto& det=ocr_res.det_dnn;
    auto& rec=ocr_res.rec_dnn;
    cv::Mat blob;

    auto output= fdnn::fcv::FaaCvOutputData();
    det.executor->preProcess(
        *det.proc,frame,blob,FaaDetPreParam{
         .width = det.width,
         .height = det.height,
         .scalefactor = 1/255.0,
    },code).
    postImage(
             det.entrance_layer,blob,*det.ctx,
             rt::mapper[det.entrance_type]
         );
    det.executor->forward(*det.ctx);
    det.executor->getOutput(
        det.exit_layer,output,*det.ctx,
        rt::mapper[det.exit_type]
        ).
    postProcess(*det.proc,
        static_cast<float*>(output.toHost()),frame, FaaDetPostParam{
         det.width,det.height,
         0.01,
         2.00,
         1,
    },code,result.result);
    for (auto rect:result.result.rects){
        cv::Mat cropped = frame(rect).clone();
        cv::Mat _blob;
        auto res_result= fdnn::fcv::processer::ocr::FaaResResult();
        rec.executor->preProcess(
                  *rec.proc,cropped,_blob, fdnn::fcv::processer::ocr::FaaResParam{
                       .width = rec.width,
                       .height = rec.height,
                       .scalefactor = 1 / 255.0
                  },
                  code
             ).postImage(rec.entrance_layer,
                       _blob,*rec.ctx,
                         rt::mapper[rec.entrance_type]
             );
        rec.executor->forward(*rec.ctx);
        rec.executor->getOutput(
                  rec.exit_layer,
                  output,*rec.ctx,
                  rt::mapper[rec.exit_type]
             ).postProcess(*rec.proc,
                  static_cast<float*>(output.toHost()),
                  fdnn::fcv::processer::ocr::FaaPostParam{
                  .model_vocal_size = 18385,
                  .seq_len = 40,
             },res_result,code
            );
            std::string text;
            std::string ids;
            for (const auto& id : res_result.ids){
                if (id-1<rec.proc->get_vocal().size()){
                    text+=rec.proc->get_vocal()[id-1];
                    ids+=std::to_string(id)+" , ";
                }
            }
            std::cout<<"old f:"<<text<<" ids:"<<ids<<std::endl;
            result.keys.emplace_back(text,rect);
    }
    delete[] static_cast<float*>(output.toHost());
}

bool faa::service::operation::set_input_win32_key(const std::string& target,
    std::map<std::string, std::unique_ptr<input::windows::WindowsInput>>& inputs,
    input::windows::InputKeyInfo info) {

    if (!inputs.contains(target)){
        if (!rt::init::win32_input_init(target,inputs)){
            return false;
        }
    }
    auto& input=inputs[target];
    input->simulateKeyPress(info);
    return true;
}

bool faa::service::operation::set_input_win32_mouse(const std::string& target,
    std::map<std::string, std::unique_ptr<input::windows::WindowsInput>>& inputs,
    input::windows::MouseInfo info,
    bool is_click)
{
    // if (!inputs.contains(target)){
    //     if (!rt::init::win32_input_init(target,inputs)){
    //         return false;
    //     }
    // }
    // auto& input=inputs[target];
    // input->activateWindow();
    // if (is_click){
    //     input->moveMouse(info);
    //     input->clickMouse(info.click_info);
    // }else{
    //     input->moveMouse(info);
    // }
    return true;
}

bool faa::service::operation::set_rotate_win32_mouse(const std::string& target,
    std::map<std::string, std::unique_ptr<input::windows::WindowsInput>>& inputs, input::windows::RotateMouseInfo info)
{
    // if (!inputs.contains(target)){
    //     if (!rt::init::win32_input_init(target,inputs)){
    //         return false;
    //     }
    // }
    // auto& input=inputs[target];
    // input->rotateMouse(info);
    return true;
}
