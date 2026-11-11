//
// Created by HP on 2026/10/31.
//
#include "faa_core/fdnn/backend/trt_cv/faa_cv_dnn_trt.h"
#include "faa_core/fdnn/backend/trt_cv/private/faa_cv_trt_framework_impl.h"
using faa::core::fdnn::backend::trt::FaaCvTensorRtBackend;

FaaCvTensorRtBackend::FaaCvTensorRtBackend()
{
    this->engine = std::make_unique<TensorRTFrameworkImpl>();

    // this->ctx    = std::make_unique<FaaCvTensorRTCtx>();
}

FaaCvTensorRtBackend::~FaaCvTensorRtBackend() = default;
FaaCvTensorRtBackend::FaaCvTensorRtBackend(FaaCvTensorRtBackend&& other) noexcept = default;
FaaCvTensorRtBackend& FaaCvTensorRtBackend::operator=(FaaCvTensorRtBackend&& other) noexcept = default;

// void FaaCvTensorRtBackend::setLogger(info::log::Ilog* logger){
//     this->logger = logger;
// }

void FaaCvTensorRtBackend::setLogger(common::ilogger::FaaIlogger* ilogger)
{
    this->ilogger = ilogger;
}


void FaaCvTensorRtBackend::setConfig(nlohmann::json& config){
    this->config = config;
}


