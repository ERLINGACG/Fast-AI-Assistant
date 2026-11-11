//
// Created by HP on 2026/11/5.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CV_OCR_REC_PROS_H
#define FASTAIASSISTANT_ROOT_FAA_CV_OCR_REC_PROS_H
#include "faa_cv_ocr_rec_param.h"
#include "faa_core/faa_export.h"
#include "faa_core/frame/faa_mat.h"

namespace  faa::core::fdnn::general::ocr {


    void FAA_CPP_EXPORT recPostProcess(
        frame::FaaMat& blob,
        frame::FaaMat& orgImg,
        const FaaRecParam& param,
        const std::vector<std::string>& vocab,
        std::string& text
    );
}
#endif //FASTAIASSISTANT_ROOT_FAA_CV_OCR_REC_PROS_H