//
// Created by HP on 2026/10/2.
//
#include "faa-core/fdnn/fcv/processer/ob/faa_yolo_proc.h"

#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
using faa::fdnn::fcv::processer::ob::FaaYoloProc;


FaaYoloProc::FaaYoloProc(const nlohmann::json& config) {
    this->config = config;
    this->class_labs = config.value("class_labs", nlohmann::json::array());
    this->numAnchors    = config.value("numAnchors", 8400);
}

void FaaYoloProc::PreProcess(const cv::Mat& img, cv::Mat& blob,const FaaYoloParams& params) {
    if (img.empty()){
        return;
    }
    blob =   cv::dnn::blobFromImage(
           img,
           1.0 / 255.0,
           cv::Size(params.input_width, params.input_height),
           cv::Scalar(0, 0, 0),
           true,
           false
    );

}

void FaaYoloProc::PostProcess(const float* output, cv::Mat& orgImg,
                              FaaYoloParams& params, FaaYoloResult& result) {

    std::cout<<"yolo post process version 1"<<std::endl;

    const int numClasses = static_cast<int>(class_labs.size());
    if (numClasses == 0 || numAnchors <= 0 || !output) return;

    const int totalChannels = 4 + numClasses;   // = 25

    const float x_factor = static_cast<float>(orgImg.cols) / params.input_width;
    const float y_factor = static_cast<float>(orgImg.rows) / params.input_height;

    std::vector<
        std::tuple<int,float,cv::Rect>
    > rects;
    for (int w = 0; w < numAnchors; ++w) {
        float maxScore = 0.0f;
        int classId = -1;

        // 遍历类别（通道 4 开始）
        for (int c = 0; c < numClasses; ++c) {
            float score = output[(4 + c) * numAnchors + w];
            if (score > maxScore) {
                maxScore = score;
                classId = c;
            }
        }

        if (maxScore <= params.maxScore || classId < 0) continue;

        float cx    = output[0 * numAnchors + w];
        float cy    = output[1 * numAnchors + w];
        float w_box = output[2 * numAnchors + w];
        float h_box = output[3 * numAnchors + w];

        int x1 = static_cast<int>((cx - w_box / 2) * x_factor);
        int y1 = static_cast<int>((cy - h_box / 2) * y_factor);
        int x2 = static_cast<int>((cx + w_box / 2) * x_factor);
        int y2 = static_cast<int>((cy + h_box / 2) * y_factor);

        // 裁剪，防止 cv::rectangle 抛异常
        x1 = std::clamp(x1, 0, orgImg.cols - 1);
        y1 = std::clamp(y1, 0, orgImg.rows - 1);
        x2 = std::clamp(x2, 0, orgImg.cols - 1);
        y2 = std::clamp(y2, 0, orgImg.rows - 1);

        rects.push_back({classId,maxScore,cv::Rect(x1, y1, x2 - x1, y2 - y1)});

    }

    if (rects.empty()){
        return;
    }
    std::map<int, std::vector<int>> clsToIdx;   // class_id -> rects 中的下标
    for (int i = 0; i < static_cast<int>(rects.size()); ++i) {
        int clsId = std::get<0>(rects[i]);
        clsToIdx[clsId].push_back(i);
    }
    // ───────── 2. 每个类别单独 NMS ─────────
    for (auto& idxList : clsToIdx | std::views::values) {
        std::vector<cv::Rect> boxes;
        std::vector<float>    scores;
        boxes.reserve (idxList.size());
        scores.reserve(idxList.size());

        for (int i : idxList) {
            boxes.push_back (std::get<2>(rects[i]));   // cv::Rect
            scores.push_back(std::get<1>(rects[i]));  // score
        }

        std::vector<int> keep;
        cv::dnn::NMSBoxes(boxes, scores,
                          params.maxScore,        // score_threshold
                          params.iou,    // nms_threshold，通常 0.45
                          keep);

        for (int k : keep) {
            const auto& t = rects[idxList[k]];
            int      cid   = std::get<0>(t);
            float    score = std::get<1>(t);
            cv::Rect rect  = std::get<2>(t);

            // 写入结果
            result.push({
                rect.x,
                rect.y,
                rect.width,
                rect.height,
                score,
                cid,
                class_labs[cid]
            });

            // 画框
            if (params.isDraw) {
                cv::rectangle(orgImg, rect, cv::Scalar(0, 255, 0), 2);
            }
        }
    }

    if (params.isDraw) {
        cv::imwrite("output.jpg", orgImg);   // 移到循环外，只写一次
    }
}
