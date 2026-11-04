//
// Created by HP on 2026/11/1.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_DATA_H
#define FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_DATA_H
#include <memory>
#include <variant>
#include <vector>

namespace faa::core::fdnn::backend::trt
{

    using FaaDataVec = std::variant<
        std::vector<float>,
        std::vector<uint16_t>
    >;

    struct FaaCvTensorRtDataImpl;

    struct FaaCvTensorRtData {
        std::unique_ptr<FaaCvTensorRtDataImpl> impl;
        FaaDataVec data;
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_DATA_H
