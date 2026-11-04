//
// Created by HP on 2026/10/31.
//
#include "faa_core/fdnn/backend/trt_cv/private/faa_cv_trt_framework_impl.h"
using faa::core::fdnn::backend::trt::TensorRTFrameworkImpl;

bool TensorRTFrameworkImpl::init(const std::string& model_path) {
    this->framework = std::make_unique<gdlz::trt::icv::core::TensorRTCvCoreFramework>(model_path.c_str());
    if (!this->framework) {
        return false;
    }
    framework->CreateEngine();
    return true;

}
