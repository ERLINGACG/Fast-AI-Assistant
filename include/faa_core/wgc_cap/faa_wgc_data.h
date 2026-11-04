//
// Created by HP on 2026/7/1.
//

#ifndef FASTSCREEN_FAA_WGC_DATA_H
#define FASTSCREEN_FAA_WGC_DATA_H
#include <cstdint>
#include "faa_core/frame/faa_mat.h"

namespace faa::core::cap::wgc {

    struct WgcTextureData {

        int32_t height;
        int32_t width;
        uint32_t RowPitch;
        uint32_t DepthPitch;
        uint8_t* data;


        void textureForMat(frame::FaaMat& mat);
    };
}
#endif //FASTSCREEN_FAA_WGC_DATA_H