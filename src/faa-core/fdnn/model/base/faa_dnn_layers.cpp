//
// Created by HP on 2026/11/4.
//
#include "faa_core/fdnn/backend/trt_cv/data/faa_cv_dnn_trt_type.h"
#include "faa_core/fdnn/model/base/faa_dnn_model_layer.h"
using  faa::core::fdnn::model::layer::FaaDnnModelInputLayer;
using  faa::core::fdnn::model::layer::FaaDnnModelOutputLayer;

void faa::core::fdnn::model::layer::build_InputLayer_to_json(FaaDnnModelInputLayer& layer, nlohmann::json& json) {

    layer.name      = json.value("name","");
    layer.dtype     = json.value("dtype",backend::trt::kFaaFloat32);
    layer.min_shape = json.value("min_shape",nlohmann::json::array()).get<std::vector<int64_t>>();
    layer.opt_shape = json.value("opt_shape",nlohmann::json::array()).get<std::vector<int64_t>>();
    layer.max_shape = json.value("max_shape",nlohmann::json::array()).get<std::vector<int64_t>>();


}

void faa::core::fdnn::model::layer::build_OutputLayer_to_json(FaaDnnModelOutputLayer& layer, nlohmann::json& json) {

    layer.name   = json.value("name","");
    layer.dtype = json.value("dtype",backend::trt::kFaaFloat32);
}
