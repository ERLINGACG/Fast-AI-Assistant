//
// Created by HP on 2026/7/1.
//

#ifndef FASTSCREEN_FAA_WGC_DATA_H
#define FASTSCREEN_FAA_WGC_DATA_H
#include <cstdint>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>

namespace faa::core::cap::wgc {

    struct WgcTextureData {

        int32_t height;
        int32_t width;
        uint32_t RowPitch;
        uint32_t DepthPitch;
        uint8_t* data;

        auto textureForMat(cv::Mat& mat) const {
            if (this->data ==nullptr) {
                return;
            }
            cv::Mat bgraMat(height, width, CV_8UC4, data, RowPitch);
            cv::cvtColor(bgraMat, mat, cv::COLOR_BGRA2BGR);
        }
    };
}
#endif //FASTSCREEN_FAA_WGC_DATA_H