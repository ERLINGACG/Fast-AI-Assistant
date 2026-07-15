//
// Created by HP on 2026/7/2.
//
#include <iostream>
#include "gdlz/trt/core/trt_core_framework.h"
#include "gdlz/trt/core/trt_core_director.h"
#include "gdlz/trt/cv/ocr/trt_cv_ocr_framework.h"
#include "gdlz/trt/core/cuda/op/trt_cuda_memory.h"
// #include <cuda_runtime.h>
using gdlz::trt::core::TensorRTCoreEngine;
using gdlz::trt::core::TensorRTCoreDirector;
using gdlz::trt::core::TensorRtBuilderShapeInfo;
using gdlz::trt::core::TensorBuilderEngineInfo;
using gdlz::trt::core::TensorRTCoreFramework;
auto build(const std::string& config_path){

    auto  engine=TensorRTCoreEngine();
    auto  engine_info=TensorBuilderEngineInfo();

    // std::vector<int64_t> x_layer={1,3,48,320};
    std::vector<int64_t> x_layer={1,3,640,640};
    auto  shap_x=TensorRtBuilderShapeInfo();
    auto  shap_fetch=TensorRtBuilderShapeInfo();

    auto  input_name=std::string("x");
    auto  fetch_name=std::string("fetch_name_0");


    shap_x.name=const_cast<char*>(input_name.c_str());
    shap_x.K_min_shape=x_layer.data();
    shap_x.K_min_dims =static_cast<int64_t>(x_layer.size());
    shap_x.K_max_shape=x_layer.data();
    shap_x.K_max_dims =static_cast<int64_t>(x_layer.size());
    shap_x.K_opt_shape=x_layer.data();
    shap_x.K_opt_dims =static_cast<int64_t>(x_layer.size());


    TensorRTCoreFramework::InitShapeInfo(engine_info);
    TensorRTCoreFramework::RegisterShapeInfo(
        shap_x,
        engine_info
    );
    TensorRTCoreFramework::BuildEngine(engine,
        config_path.c_str(),
        engine_info
    );
}

auto test_run(const std::string& config_path){
     using gdlz::trt::icv::ocr::TensorRTCvOCRFramework;
     auto img = cv::imread(R"(./image/test2.png)");
     if (img.empty()){
        std::cerr<<"img is empty" << std::endl;
        return;
     }
    std::cout << "=== img: " << img.cols << "x" << img.rows << std::endl;

    // 直接 resize 到 640x640（BGR, uint8 — 归一化用 scalefactor=/255，无 mean/std）
    cv::Mat resized;
    cv::resize(img, resized, cv::Size(640, 640));

    // ── 推理 Lambda：输入 uint8 BGR 图，/255 归一化到 [0,1] ──
    auto runInference = [&](const cv::Mat& input640, const char* tag)
        -> std::pair<cv::Mat, std::vector<cv::RotatedRect>> {
        cv::Mat blob;
        // Ascend 模型可能期望 BGR，先试 swapRB=false
        cv::dnn::blobFromImage(input640, blob, 1.0 / 255.0, cv::Size(640, 640), cv::Scalar(), false, false);

        auto ocr_framework = TensorRTCvOCRFramework(config_path.c_str());
        auto ocr_ctx = gdlz::trt::icv::ocr::TensorRTCvOCRCtx();
        auto out_put = gdlz::trt::core::cuda::TensorRTCudaData();
        ocr_framework.CreateEngine();
        ocr_framework.GetEngineInfo();
        ocr_framework.CreateCtx(ocr_ctx);

        std::vector<int64_t> x_layer = {1,3,640,640};
        TensorRTCvOCRFramework::RegisterInputLager(ocr_ctx,"x",x_layer.data(),static_cast<int64_t>(x_layer.size()));
        TensorRTCvOCRFramework::RegisterOutputLager(ocr_ctx,"fetch_name_0",gdlz::trt::core::cuda::DataType::FLOAT32);
        TensorRTCvOCRFramework::PostImg("x",ocr_ctx,blob,gdlz::trt::core::cuda::DataType::FLOAT32);

        auto t0 = std::chrono::high_resolution_clock::now();
        TensorRTCvOCRFramework::Forward(ocr_ctx);
        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "[" << tag << "] forward: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << "ms" << std::endl;

        TensorRTCvOCRFramework::GetOutput("fetch_name_0",ocr_ctx,out_put,gdlz::trt::core::cuda::DataType::FLOAT32);

        // 复制 probMap
        auto* outputHost = static_cast<float*>(out_put.data);
        cv::Mat probMap(640, 640, CV_32F);
        cv::Mat(640, 640, CV_32F, outputHost).copyTo(probMap);

        double pmin, pmax;
        cv::minMaxLoc(probMap, &pmin, &pmax);
        std::cout << "[" << tag << "] probMap range: [" << pmin << ", " << pmax << "]" << std::endl;

        // 归一化 probMap 到 [0, 1]，解决 Ascend 模型输出值域偏低的问题
        if (pmax > pmin) {
            probMap = (probMap - pmin) / (pmax - pmin);
        }

        // 从 probMap 提取轮廓
        auto extractBoxes = [](const cv::Mat& prob, float thresh) -> std::vector<cv::RotatedRect> {
            cv::Mat bin;
            cv::threshold(prob, bin, thresh, 1.0, cv::THRESH_BINARY);
            bin.convertTo(bin, CV_8U, 255);
            std::vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;
            cv::findContours(bin, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            std::vector<cv::RotatedRect> boxes;
            for (const auto& c : contours) {
                if (cv::contourArea(c) < 50) continue;
                boxes.push_back(cv::minAreaRect(c));
            }
            return boxes;
        };

        auto boxes = extractBoxes(probMap, 0.3f);
        std::cout << "[" << tag << "] found " << boxes.size() << " boxes" << std::endl;
        return {probMap, boxes};
    };

    // ── 第一遍：归一化后推理 ──
    auto [probMap, textBoxes] = runInference(resized, "normal");

    // ── 极性检测 ──
    cv::Mat binCheck;
    cv::threshold(probMap, binCheck, 0.3, 1.0, cv::THRESH_BINARY);
    binCheck.convertTo(binCheck, CV_8U, 255);
    int whitePixels = cv::countNonZero(binCheck);
    double whiteRatio = static_cast<double>(whitePixels) / (640.0 * 640.0);
    std::cout << "binary white ratio: " << (whiteRatio * 100.0) << "%" << std::endl;

    if (whiteRatio > 0.5) {
        std::cout << "Polarity inverted (background as text), flipping probMap..." << std::endl;
        probMap = 1.0f - probMap;
        cv::Mat binFlipped;
        cv::threshold(probMap, binFlipped, 0.3, 1.0, cv::THRESH_BINARY);
        binFlipped.convertTo(binFlipped, CV_8U, 255);
        cv::imwrite("binary_output.png", binFlipped);
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(binFlipped, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        textBoxes.clear();
        for (const auto& c : contours) {
            if (cv::contourArea(c) < 50) continue;
            textBoxes.push_back(cv::minAreaRect(c));
        }
        std::cout << "after flip: " << textBoxes.size() << " boxes" << std::endl;
    } else if (whiteRatio < 0.01 && textBoxes.empty()) {
        std::cout << "probMap nearly black, trying inverted input..." << std::endl;
        cv::Mat inverted;
        cv::bitwise_not(resized, inverted);     // 反色 uint8 原图
        std::tie(probMap, textBoxes) = runInference(inverted, "inverted");
    }

    // ── 逆变换：640x640 坐标 → 原始图像坐标 ──
    cv::Mat result = img.clone();
    float sx = static_cast<float>(img.cols) / 640.0f;
    float sy = static_cast<float>(img.rows) / 640.0f;
    for (const auto& rect : textBoxes) {
        cv::Point2f vertices[4];
        rect.points(vertices);
        for (int i = 0; i < 4; ++i) {
            float orig_x = vertices[i].x * sx;
            float orig_y = vertices[i].y * sy;
            vertices[i].x = std::max(0.0f, std::min(orig_x, static_cast<float>(img.cols - 1)));
            vertices[i].y = std::max(0.0f, std::min(orig_y, static_cast<float>(img.rows - 1)));
        }
        for (int i = 0; i < 4; ++i) {
            cv::line(result, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
        }
    }
    cv::imwrite("detection_result.png", result);
    std::cout << "Saved detection_result.png  total=" << textBoxes.size() << " boxes" << std::endl;
}

auto test_v2(const std::string& config_path){
    using gdlz::trt::icv::ocr::TensorRTCvOCRFramework;
    auto img = cv::imread(R"(./image/test1.png)");
    if (img.empty()){
        std::cerr<<"img is empty" << std::endl;
        return;
    }
    std::cout << "=== img: " << img.cols << "x" << img.rows << std::endl;

    cv::Mat resized;
    cv::resize(img, resized, cv::Size(640, 640));

    cv::Mat blob;
    cv::dnn::blobFromImage(resized, blob, 1.0/255, cv::Size(640, 640), cv::Scalar(), true, false);

    auto ocr_framework = TensorRTCvOCRFramework(config_path.c_str());
    auto ocr_ctx = gdlz::trt::icv::ocr::TensorRTCvOCRCtx();
    auto out_put = gdlz::trt::core::cuda::TensorRTCudaData();
    ocr_framework.CreateEngine();
    ocr_framework.GetEngineInfo();
    ocr_framework.CreateCtx(ocr_ctx);

    std::vector<int64_t> x_layer = {1,3,640,640};
    TensorRTCvOCRFramework::RegisterInputLager(ocr_ctx,"x",x_layer.data(),static_cast<int64_t>(x_layer.size()));
    TensorRTCvOCRFramework::RegisterOutputLager(ocr_ctx,"fetch_name_0",gdlz::trt::core::cuda::DataType::FLOAT32);
    TensorRTCvOCRFramework::PostImg("x",ocr_ctx,blob,gdlz::trt::core::cuda::DataType::FLOAT32);

    auto t0 = std::chrono::high_resolution_clock::now();
    TensorRTCvOCRFramework::Forward(ocr_ctx);
    auto t1 = std::chrono::high_resolution_clock::now();
    std::cout << "forward: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << "ms" << std::endl;

    TensorRTCvOCRFramework::GetOutput("fetch_name_0",ocr_ctx,out_put,gdlz::trt::core::cuda::DataType::FLOAT32);

    auto* outputHost = static_cast<float*>(out_put.data);
    cv::Mat probMap(640, 640, CV_32F);
    cv::Mat(640, 640, CV_32F, outputHost).copyTo(probMap);

    double pmin, pmax;
    cv::minMaxLoc(probMap, &pmin, &pmax);
    std::cout << "probMap range: [" << pmin << ", " << pmax << "]" << std::endl;

    // 归一化到 [0, 1]，使阈值稳定
    if (pmax > pmin) {
        probMap = (probMap - pmin) / (pmax - pmin);
    }

    // 二值化（阈值 0.1 减少漏检）
    cv::Mat bin;
    cv::threshold(probMap, bin, 0.1, 1.0, cv::THRESH_BINARY);
    bin.convertTo(bin, CV_8U, 255);
    cv::imwrite("binary_output.png", bin);

    // 提取轮廓
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(bin, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // DBNet unclip 膨胀比：输出的是 shrunk kernel，需放大还原
    const float unclip_ratio = 2.0f;
    // 面积过滤：相对于 640x640 图像的比例（自适应图片大小）
    const double min_area = std::max(5.0, 30.0 * img.cols * img.rows / (640.0 * 640.0));

    std::vector<cv::RotatedRect> textBoxes;
    for (const auto& c : contours) {
        double area = cv::contourArea(c);
        if (area < min_area) continue;
        cv::RotatedRect r = cv::minAreaRect(c);
        // unclip 膨胀：放大宽高，保持中心和角度不变
        r.size.width  *= unclip_ratio;
        r.size.height *= unclip_ratio;
        textBoxes.push_back(r);
    }
    std::cout << "found " << textBoxes.size() << " boxes (min_area=" << min_area << ")" << std::endl;

    // 逆变换 + 画框
    cv::Mat result = img.clone();
    float sx = static_cast<float>(img.cols) / 640.0f;
    float sy = static_cast<float>(img.rows) / 640.0f;
    for (const auto& rect : textBoxes) {
        cv::Point2f vertices[4];
        rect.points(vertices);
        for (int i = 0; i < 4; ++i) {
            vertices[i].x = std::max(0.0f, std::min(vertices[i].x * sx, static_cast<float>(img.cols - 1)));
            vertices[i].y = std::max(0.0f, std::min(vertices[i].y * sy, static_cast<float>(img.rows - 1)));
        }
        for (int i = 0; i < 4; ++i) {
            cv::line(result, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
        }
    }
    cv::imwrite("detection_result.png", result);
    std::cout << "Saved detection_result.png  total=" << textBoxes.size() << " boxes" << std::endl;
}

int main() {


    build("config/trt_ocr.json");
    // test_run("config/trt_ocr.json");
    // test_v2("config/trt_ocr.json");
    return 0;
}