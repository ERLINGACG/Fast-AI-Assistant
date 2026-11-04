//
// Created by HP on 2026/11/1.
//
#include "faa_core/fdnn/general/ocr/faa_cv_ocr_det_pros.h"
#include "faa_core/frame/private/faa_mat_impi.h"
#include <opencv2/opencv.hpp>
using faa::core::frame::FaaMat;
using faa::core::frame::FaaMatImpl;



void faa::core::fdnn::general::ocr::detPostProcess(
        FaaMat& blob,
        FaaMat& orgImg,
        int width,
        int height,
        const FaaDetParam& param,
        FaaDetResults& results)
{
    if (blob.impl()->mat.empty() || orgImg.impl()->mat.empty()) return;

    auto* output = blob.impl()->mat.ptr<float>(0);
    cv::Mat& src = orgImg.impl()->mat;



    // 1. 直接构造二维概率图，🌟 绝对不要做 MinMax 归一化！
    const cv::Mat probMap(height, width, CV_32F, output);

    // 2. 二值化（如果框残缺，可以把阈值降到 0.2，目前先用 0.3）
    cv::Mat bin;
    cv::threshold(probMap, bin, param.threshold, 255, cv::THRESH_BINARY);
    bin.convertTo(bin, CV_8U);
    if (param.save_bin) {
        cv::imwrite("bin.png", bin);
    }

    // 3. 轻微膨胀（使用 3x3 或直接去掉，千万别用 morphologyEx 闭运算）
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::dilate(bin, bin, kernel);

    // 4. 找轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(bin, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 5. 计算反向映射比例（暴力拉伸的 X/Y 比例）
    float sx = static_cast<float>(src.cols) / static_cast<float>(width);
    float sy = static_cast<float>(src.rows) / static_cast<float>(height);

    for (const auto& c : contours) {
        // 过滤太小的噪点
        if (cv::contourArea(c) < 10.0f) continue;

        // 🌟 关键：使用 boundingRect 而不是 minAreaRect
        // 因为拉伸后的字变形了，求旋转矩形会导致框极度怪异
        cv::Rect r = cv::boundingRect(c);

        // 映射回原图坐标
        cv::Rect bbox;
        bbox.x      = static_cast<int>(r.x * sx);
        bbox.y      = static_cast<int>(r.y * sy);
        bbox.width  = static_cast<int>(r.width * sx);
        bbox.height = static_cast<int>(r.height * sy);

        // 🌟 极小幅度的 Padding（只留 2 像素白边防止切字，不要乘倍数）
        int pad = param.pad;
        bbox.x      = std::max(0, bbox.x - pad);
        bbox.y      = std::max(0, bbox.y - pad);
        bbox.width  = std::min(src.cols - bbox.x, bbox.width + pad * 2);
        bbox.height = std::min(src.rows - bbox.y, bbox.height + pad * 2);

        if (bbox.width <= 0 || bbox.height <= 0) continue;
        results.items.push_back({
            bbox.x,
            bbox.y,
            bbox.width,
            bbox.height
        }
        );

        // 画蓝框
        cv::rectangle(src, bbox, cv::Scalar(255, 0, 0), 2);
    }
}