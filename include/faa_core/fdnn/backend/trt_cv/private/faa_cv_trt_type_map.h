//
// Created by HP on 2026/10/31.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_TYPE_H
#define FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_TYPE_H
#include <string>
#include <unordered_map>
#include <gdlz/trt/core/trt_core_director.h>
#include <gdlz/trt/core/cuda/buffer/trt_cuda_buffer_type.h>
#include "../data/faa_cv_dnn_trt_type.h"
namespace faa::core::fdnn::backend::trt {

    static std::unordered_map<std::string,gdlz::trt::core::cuda::DataType> type_map {
        {kFaaFloat32,gdlz::trt::core::cuda::DataType::FLOAT32},
        {kFaaFloat16,gdlz::trt::core::cuda::DataType::FLOAT16},
    };

    static std::unordered_map<std::string,gdlz::trt::core::cuda::BufferType> type_map_v2 {
        {kFaaFloat32,gdlz::trt::core::cuda::kFloat32{}},
        {kFaaFloat16,gdlz::trt::core::cuda::kFloat16{}},

    };


}
#endif //FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_TYPE_H