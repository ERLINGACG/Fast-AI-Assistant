//
// Created by HP on 2026/10/3.
//

#ifndef FASTSCREEN_FAA_DNN_DATA_TYPE_H
#define FASTSCREEN_FAA_DNN_DATA_TYPE_H
#include <map>
#include <string>
#include <gdlz/trt/core/cuda/trt_cuda_data.h>

namespace  faa::fdnn::data::type{

    inline  std::map<std::string,gdlz::trt::core::cuda::DataType> fdnn_types_mapping {
                      {"fp32",gdlz::trt::core::cuda::DataType::FLOAT32},
                      {"fp16",gdlz::trt::core::cuda::DataType::FLOAT16},
                      {"int8",gdlz::trt::core::cuda::DataType::INT8},
    };
}
#endif //FASTSCREEN_FAA_DNN_DATA_TYPE_H