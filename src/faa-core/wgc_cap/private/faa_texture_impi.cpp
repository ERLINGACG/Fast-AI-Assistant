//
// Created by HP on 2026/10/20.
//
#include <opencv2/imgproc.hpp>

#include "faa_core/frame/private/faa_mat_impi.h"
#include "faa_core/wgc_cap/faa_wgc_data.h"
using faa::core::cap::wgc::WgcTextureData;

void WgcTextureData::textureForMat(frame::FaaMat& mat) {

    const cv::Mat org(height, width, CV_8UC4, data, RowPitch);
    cv::cvtColor(org, mat.impl()->mat, cv::COLOR_BGRA2BGR);
}
