//
// Created by HP on 2026/11/4.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_DNN_MODEL_LAYER_H
#define FASTAIASSISTANT_ROOT_FAA_DNN_MODEL_LAYER_H
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "faa_core/faa_export.h"
namespace faa::core::fdnn::model::layer {

    struct FaaDnnModelInputLayer {

        std::string name;
        std::string dtype;

        std::vector<int64_t> min_shape;
        std::vector<int64_t> opt_shape;
        std::vector<int64_t> max_shape;

        [[nodiscard]] bool is_dynamic() const {
            return min_shape == max_shape && opt_shape == max_shape ;
        };


    };


    struct FaaDnnModelOutputLayer{

        std::string name;
        std::string dtype;
    };

    void FAA_CPP_EXPORT build_InputLayer_to_json (FaaDnnModelInputLayer& layer,nlohmann::json& json);
    void FAA_CPP_EXPORT build_OutputLayer_to_json(FaaDnnModelOutputLayer& layer,nlohmann::json& json);
}
#endif //FASTAIASSISTANT_ROOT_FAA_DNN_MODEL_LAYER_H