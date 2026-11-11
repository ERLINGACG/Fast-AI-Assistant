//
// Created by HP on 2026/11/6.
//
#include "faa_core/fdnn/general/mixformer/faa_cv_mixformer_v2_pros.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ostream>
#include <stdexcept>

#include "faa_core/frame/private/faa_mat_impi.h"


void faa::core::fdnn::general::mixformer_v2::mixFormerPresImg(
    const PerParam& param,
    const frame::FaaMat& org,
    std::vector<std::pair<frame::FaaMat, RecBox>>& imgs) {
    imgs.clear();

    constexpr int kWin = 224;
    const int H = static_cast<int>(org.row());
    const int W = static_cast<int>(org.col());
    if (W < kWin || H < kWin) return;

    const int startX = std::clamp(param.copy_x, 0, W - kWin);
    const int startY = std::clamp(param.copy_y, 0, H - kWin);

    int searchW = (param.copy_w > 0) ? param.copy_w : kWin;
    int searchH = (param.copy_h > 0) ? param.copy_h : kWin;
    searchW = std::min(searchW, W - startX);
    searchH = std::min(searchH, H - startY);

    const int step = (param.step > 0) ? param.step : kWin;

    for (int y = startY; y + kWin <= startY + searchH; y += step) {
        for (int x = startX; x + kWin <= startX + searchW; x += step) {
            cv::Mat roi = org.impl()->mat(cv::Rect(x, y, kWin, kWin)).clone();

            frame::FaaMat roiMat;
            roiMat.impl()->mat = roi;

            imgs.emplace_back(std::move(roiMat), RecBox{x, y, kWin, kWin});
        }
    }
}


void faa::core::fdnn::general::mixformer_v2::mixFormerPostprocess(
    frame::FaaMat& org,
    frame::FaaMat& roi_org,
    frame::FaaMat& pred_boxes_blob,
    frame::FaaMat& pred_scores_blob,
    const RecBox& cropBox)
{
    auto* pred_boxes  = pred_boxes_blob.impl()->mat.ptr<float>();
    auto* pred_scores = pred_scores_blob.impl()->mat.ptr<float>();

    float cx_norm = pred_boxes[0];
    float cy_norm = pred_boxes[1];
    float w_norm  = pred_boxes[2];
    float h_norm  = pred_boxes[3];
    float score   = pred_scores[0];

    // 逆映射：(norm * crop尺寸) + crop偏移
    float x1_org = (cx_norm - w_norm * 0.5f) * cropBox.w + cropBox.x;
    float y1_org = (cy_norm - h_norm * 0.5f) * cropBox.h + cropBox.y;
    float w_org  = w_norm * cropBox.w;
    float h_org  = h_norm * cropBox.h;

    std::cout << "score:" << score
              << " | crop: [" << cropBox.x << "," << cropBox.y << ","
              << cropBox.w << "," << cropBox.h << "]"
              << " | org_box: [" << x1_org << "," << y1_org << ","
              << w_org << "," << h_org << "]" << std::endl;

    cv::Mat& draw_mat = org.impl()->mat;

    int draw_x = std::max(0, (int)std::round(x1_org));
    int draw_y = std::max(0, (int)std::round(y1_org));
    int draw_w = std::min(draw_mat.cols - draw_x, (int)std::round(w_org));
    int draw_h = std::min(draw_mat.rows - draw_y, (int)std::round(h_org));

    if (score > 0.8 && draw_w > 0 && draw_h > 0) {
        cv::rectangle(draw_mat, cv::Rect(draw_x, draw_y, draw_w, draw_h),
                      cv::Scalar(255, 0, 0), 2);
        std::string text = "Score: " + std::to_string(score).substr(0, 5);
        cv::putText(draw_mat, text, cv::Point(draw_x, std::max(0, draw_y - 5)),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
    }

    cv::imwrite("debug_org_with_blue_box.png", draw_mat);
}
// ---------- 辅助函数：计算两个矩形的 IoU ----------
using RecBox = faa::core::fdnn::general::mixformer_v2::RecBox;
static float calcIoU(const RecBox& a, const RecBox& b) {
    int x1 = std::max(a.x, b.x);
    int y1 = std::max(a.y, b.y);
    int x2 = std::min(a.x + a.w, b.x + b.w);
    int y2 = std::min(a.y + a.h, b.y + b.h);

    int w = std::max(0, x2 - x1);
    int h = std::max(0, y2 - y1);
    float inter_area = w * h;

    float area_a = a.w * a.h;
    float area_b = b.w * b.h;
    return inter_area / (area_a + area_b - inter_area + 1e-6f);
}

std::vector<faa::core::fdnn::general::mixformer_v2::MixformerDet> faa::core::fdnn::general::mixformer_v2::mixFormerNMS(
    const std::vector<MixformerDet>& dets,
    float score_thresh,
    float nms_thresh
    ) {
    std::vector<MixformerDet> valid_dets;
    for (const auto& d : dets) {
        if (d.score > score_thresh) valid_dets.push_back(d);
    }

    // 按分数从大到小排序
    std::sort(valid_dets.begin(), valid_dets.end(),
        [](const MixformerDet& a, const MixformerDet& b) {
            return a.score > b.score;
        });

    std::vector<MixformerDet> result;
    std::vector<bool> suppressed(valid_dets.size(), false);

    for (size_t i = 0; i < valid_dets.size(); ++i) {
        if (suppressed[i]) continue;
        result.push_back(valid_dets[i]);

        for (size_t j = i + 1; j < valid_dets.size(); ++j) {
            if (suppressed[j]) continue;
            // 如果重叠度超过阈值，抑制掉
            if (calcIoU(valid_dets[i].box, valid_dets[j].box) > nms_thresh) {
                suppressed[j] = true;
            }
        }
    }
    return result;
}

void faa::core::fdnn::general::mixformer_v2::mixFormerDraw(frame::FaaMat& org, const std::vector<MixformerDet>& dets,
    const std::string& save_path)
{
    cv::Mat& draw_mat = org.impl()->mat; // 这里才碰 cv::Mat
    for (const auto& det : dets) {
        cv::Rect r(det.box.x, det.box.y, det.box.w, det.box.h);
        cv::rectangle(draw_mat, r, cv::Scalar(255, 0, 0), 2);

        std::string text = "Score: " + std::to_string(det.score).substr(0, 5);
        cv::putText(draw_mat, text,
                    cv::Point(r.x, std::max(0, r.y - 5)),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
    }

    if (!save_path.empty()) {
        cv::imwrite(save_path, draw_mat);
    }
}

void faa::core::fdnn::general::mixformer_v2::mixFormerPostprocess(
    const frame::FaaMat& org,
    frame::FaaMat& pred_boxes_blob,
    frame::FaaMat& pred_scores_blob, const RecBox& cropBox,
    std::vector<MixformerDet>& out_dets)
{
    auto* pred_boxes  = pred_boxes_blob.impl()->mat.ptr<float>();
    auto* pred_scores = pred_scores_blob.impl()->mat.ptr<float>();

    float cx_norm = pred_boxes[0];
    float cy_norm = pred_boxes[1];
    float w_norm  = pred_boxes[2];
    float h_norm  = pred_boxes[3];
    float score   = pred_scores[0];

    // 逆映射
    float x1_org = (cx_norm - w_norm * 0.5f) * cropBox.w + cropBox.x;
    float y1_org = (cy_norm - h_norm * 0.5f) * cropBox.h + cropBox.y;
    float w_org  = w_norm * cropBox.w;
    float h_org  = h_norm * cropBox.h;

    int draw_x = std::max(0, (int)std::round(x1_org));
    int draw_y = std::max(0, (int)std::round(y1_org));
    int draw_w = std::min((int)org.col() - draw_x, (int)std::round(w_org));
    int draw_h = std::min((int)org.row() - draw_y, (int)std::round(h_org));

    // 只要合法就存起来，阈值留给 NMS 统一做
    if (draw_w > 0 && draw_h > 0) {
        out_dets.push_back({RecBox{draw_x, draw_y, draw_w, draw_h}, score});
    }
}
