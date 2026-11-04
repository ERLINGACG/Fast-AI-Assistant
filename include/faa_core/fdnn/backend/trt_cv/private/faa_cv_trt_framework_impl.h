//
// Created by HP on 2026/10/31.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_IMPL_H
#define FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_IMPL_H
#include <memory>
// #include <gdlz/trt/core/trt_core_director.h>
#include <gdlz/trt/cv/core/trt_cv_core.h>

namespace faa::core::fdnn::backend::trt
{
    struct TensorRTFrameworkImpl
    {
        std::unique_ptr<gdlz::trt::icv::core::TensorRTCvCoreFramework> framework;

        bool  init(const std::string& model_path);
        auto& getFramework() const { return *framework; }
        bool  isInit() const { if (framework) return true; return false; }
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_IMPL_H