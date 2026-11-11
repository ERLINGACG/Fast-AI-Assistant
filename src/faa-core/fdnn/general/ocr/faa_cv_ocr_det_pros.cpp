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
    results.items.clear();
    if (blob.impl()->mat.empty() || orgImg.impl()->mat.empty()) return;

    auto* output = blob.impl()->mat.ptr<float>(0);
    cv::Mat& src = orgImg.impl()->mat;

    // 1. 绝对不要做 MinMax 归一化！直接构造
    const cv::Mat probMap(height, width, CV_32F, output);

    // 2. 二值化
    cv::Mat bin;
    cv::threshold(probMap, bin, param.threshold, 255, cv::THRESH_BINARY);
    bin.convertTo(bin, CV_8U);

    // 3. 轻微膨胀（3x3 即可，防止字内部断裂，但不要粘连不同行）
    static cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::dilate(bin, bin, kernel);

    // 4. 找轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(bin, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 5. 计算反向映射比例
    float sx = static_cast<float>(src.cols) / static_cast<float>(width);
    float sy = static_cast<float>(src.rows) / static_cast<float>(height);

    for (const auto& c : contours) {
        if (cv::contourArea(c) < 10.0f) continue;

        // 🌟 关键：回归 boundingRect，不放大，防变形
        cv::Rect r = cv::boundingRect(c);

        cv::Rect rClamped = r & cv::Rect(0, 0, width, height);
        if (rClamped.width <= 0 || rClamped.height <= 0) continue;

        // 算 Score
        cv::Mat mask = cv::Mat::zeros(rClamped.size(), CV_8U);
        std::vector<cv::Point> shifted;
        shifted.reserve(c.size());
        for (const auto& p : c) {
            shifted.emplace_back(p.x - rClamped.x, p.y - rClamped.y);
        }
        std::vector<std::vector<cv::Point>> polys{ shifted };
        cv::fillPoly(mask, polys, cv::Scalar(1));
        float score = static_cast<float>(cv::mean(probMap(rClamped), mask)[0]);

        // 🌟 核心清洗 1：Score 过滤
        // 图中腰部的竖线、右侧边缘的线，分数通常很低，这里直接拦截
        if (score < 0.6f) continue;

        // 映射回原图
        cv::Rect bbox;
        bbox.x      = static_cast<int>(r.x * sx);
        bbox.y      = static_cast<int>(r.y * sy);
        bbox.width  = static_cast<int>(r.width * sx);
        bbox.height = static_cast<int>(r.height * sy);

        // Padding
        int pad = param.pad;
        bbox.x      = std::max(0, bbox.x - pad);
        bbox.y      = std::max(0, bbox.y - pad);
        bbox.width  = std::min(src.cols - bbox.x, bbox.width + pad * 2);
        bbox.height = std::min(src.rows - bbox.y, bbox.height + pad * 2);

        if (bbox.width <= 0 || bbox.height <= 0) continue;

        // 🌟 核心清洗 2：形状硬过滤（干掉图里那些奇怪的长条）
        float ar = static_cast<float>(bbox.width) / bbox.height;
        // 去除极端长条（比如人物腰部的竖线，宽高比通常 > 10 或 < 0.1）
        if (ar > 10.0f || ar < 0.1f) continue;
        // 去除极小的噪点碎片
        if (bbox.height < 10 || bbox.width < 5) continue;

        results.items.push_back({
            bbox.x,
            bbox.y,
            bbox.width,
            bbox.height,
            score
        });
    }
}

void faa::core::fdnn::general::ocr::drawDetResults(FaaMat& orgImg, const FaaDetResults& results, bool is_save)
{
    if (results.items.empty()) return;
    cv::Mat src = orgImg.impl()->mat.clone();
    for (const auto& rec_box : results.items) {
        cv::Rect bbox(rec_box.x, rec_box.y, rec_box.w, rec_box.h);
        cv::rectangle(src, bbox, cv::Scalar(255, 0, 0), 2);
    }
    if (is_save) {
        cv::imwrite("det.png", src);
    }

}
