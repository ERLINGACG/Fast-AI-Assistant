//
// Created by HP on 2026/11/3.
//
#ifndef FASTAIASSISTANT_ROOT_FAA_CV_SET_BLOB_H
#define FASTAIASSISTANT_ROOT_FAA_CV_SET_BLOB_H
#include "faa_core/faa_export.h"
#include "faa_core/frame/faa_mat.h"

namespace faa::core::fdnn::general::blob {



    struct FaaMean {
        double b = 0;
        double g = 0;
        double r = 0;
    };


    struct FaaBlobParam {

        FaaMean mean;
        int h{};
        int w{};
        double scalefactor = 1.0;
        bool swapRB = false;
        bool crop   = false;

    };





    void FAA_CPP_EXPORT setBlob(frame::FaaMat& blob, frame::FaaMat& orgImg,const FaaBlobParam& param);


}
#endif //FASTAIASSISTANT_ROOT_FAA_CV_SET_BLOB_H