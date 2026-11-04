//
// Created by HP on 2026/10/31.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CV_TRT_CTX_H
#define FASTAIASSISTANT_ROOT_FAA_CV_TRT_CTX_H
#include <gdlz/trt/cv/core/trt_cv_core_data.h>

namespace faa::core::fdnn::backend::trt {

    struct FaaCvTensorRTCtxImpl {
        gdlz::trt::icv::core::TensorRTCvCtx ctx;
    };

}
#endif //FASTAIASSISTANT_ROOT_FAA_CV_TRT_CTX_H
