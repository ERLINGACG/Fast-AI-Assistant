//
// Created by HP on 2026/11/1.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CV_OCR_PROS_H
#define FASTAIASSISTANT_ROOT_FAA_CV_OCR_PROS_H
#include "faa_cv_ocr_det_param.h"
#include "faa_core/faa_export.h"
#include "faa_core/frame/faa_mat.h"
namespace faa::core::fdnn::general::ocr {


    void FAA_CPP_EXPORT detPostProcess(
        frame::FaaMat& blob,
        frame::FaaMat& orgImg,
        int width,
        int height,
        const FaaDetParam& param,
        FaaDetResults& results
    );
}
#endif //FASTAIASSISTANT_ROOT_FAA_CV_OCR_PROS_H