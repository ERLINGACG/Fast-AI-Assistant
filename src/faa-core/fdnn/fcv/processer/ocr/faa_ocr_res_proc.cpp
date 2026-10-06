//
// Created by HP on 2026/7/16.
//
#include <opencv2/opencv.hpp>

#include "faa-core/fdnn/fcv/processer/ocr/faa_ocr_rec_proc.h"
#include "faa-core/fdnn/runtime/faa_dnn_rt_code.h"

using faa::fdnn::fcv::processer::ocr::FaaOcrRecProc;

FaaOcrRecProc::FaaOcrRecProc(const std::string& vocal_path){
    vocal_file.open(vocal_path);
    if (!vocal_file.is_open()){
        throw std::runtime_error("Failed to open file: " + vocal_path);
    }
    std::string line;
    while (std::getline(vocal_file, line)) {
        vocal.push_back(line);
    }
}

void FaaOcrRecProc::PreProcess(cv::Mat& img, cv::Mat& blob, FaaResParam param, rt::code::FaaDnnRtCode& code) {
    if (img.empty()) { code = rt::code::FaaDnnRtCode::Failed; return; }

    // 此时 img 已经是 RGB 三通道，尺寸为 (target_w, 48)
    cv::Mat float_img;
    img.convertTo(float_img, CV_32F, 1.0 / 255.0); // [0,1]

    // PaddleOCR 识别模型常用归一化：mean=0.5, std=0.5（所有通道相同）
    cv::Scalar mean(0.5f, 0.5f, 0.5f);
    cv::Scalar std(0.5f, 0.5f, 0.5f);
    cv::Mat normalized;
    cv::subtract(float_img, mean, normalized);
    cv::divide(normalized, std, normalized);

    // 转为 blob，尺寸使用传入的 param（已经正确）
    cv::dnn::blobFromImage(normalized, blob, 1.0,
                           cv::Size(param.width, param.height),
                           cv::Scalar(0, 0, 0), false, false);
    code = rt::code::FaaDnnRtCode::Success;
}

void FaaOcrRecProc::PostProcess(float* output,FaaPostParam param,FaaResResult& result, rt::code::FaaDnnRtCode& code)
{
    // result.ids.clear();
    // for (int t = 0; t < param.seq_len; ++t) {
    //     // 找到当前时间步的最大值索引
    //     int max_idx = 0;
    //     int next_idx = 0;
    //     float max_val = -1.0f;
    //     float* time_step_data = output + t * param.model_vocal_size;
    //     for (int i = 0; i < param.model_vocal_size; ++i) {
    //         if (time_step_data[i] > max_val) {
    //             max_val = time_step_data[i];
    //             max_idx = i;
    //         }
    //     }
    //     if (max_idx!=0){
    //         result.ids.push_back(max_idx);
    //     }
    // }
    result.ids.clear();

    const int blank_id = 0;   // PP-OCR CTC blank
    int last_idx = blank_id;

    for (int t = 0; t < param.seq_len; ++t) {
        float* time_step_data = output + t * param.model_vocal_size;

        int   max_idx = 0;
        float max_val = -std::numeric_limits<float>::infinity();
        for (int i = 0; i < param.model_vocal_size; ++i) {
            if (time_step_data[i] > max_val) {
                max_val = time_step_data[i];
                max_idx = i;
            }
        }

        if (max_idx != blank_id && max_idx != last_idx) {
            result.ids.push_back(max_idx);
        }
        last_idx = max_idx;
    }

    code=rt::code::FaaDnnRtCode::Success;
}


