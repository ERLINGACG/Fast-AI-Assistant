//
// Created by HP on 2026/10/31.
//
#include "faa_core/fdnn/backend/trt_cv/private/faa_cv_trt_ctx_impl.h"
#include "faa_core/fdnn/backend/trt_cv/faa_cv_dnn_trt_ctx.h"
using faa::core::fdnn::backend::trt::FaaCvTensorRTCtxImpl;
using faa::core::fdnn::backend::trt::FaaCvTensorRTCtx;

FaaCvTensorRTCtx::FaaCvTensorRTCtx() {
    this->ctx = std::make_unique<FaaCvTensorRTCtxImpl>();
}

FaaCvTensorRTCtx::~FaaCvTensorRTCtx()= default;
FaaCvTensorRTCtx::FaaCvTensorRTCtx(FaaCvTensorRTCtx&& other) noexcept = default;
FaaCvTensorRTCtx& FaaCvTensorRTCtx::operator=(FaaCvTensorRTCtx&& other) noexcept = default;
