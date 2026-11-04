//
// Created by HP on 2026/10/31.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_CTX_H
#define FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_CTX_H
#include <memory.h>
#include "faa_core/faa_export.h"
namespace faa::core::fdnn::backend::trt {

    struct FaaCvTensorRTCtxImpl;

    struct FAA_CPP_EXPORT FaaCvTensorRTCtx {

        std::unique_ptr<FaaCvTensorRTCtxImpl> ctx;

        [[nodiscard]] auto& getCtx() const{
            return *ctx;
        }
        FaaCvTensorRTCtx();
        ~FaaCvTensorRTCtx();
        FaaCvTensorRTCtx(FaaCvTensorRTCtx&&) noexcept;
        FaaCvTensorRTCtx (const FaaCvTensorRTCtx&) = delete;
        FaaCvTensorRTCtx& operator=(FaaCvTensorRTCtx&&) noexcept;
        FaaCvTensorRTCtx& operator=(const FaaCvTensorRTCtx&) = delete;

    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_CTX_H