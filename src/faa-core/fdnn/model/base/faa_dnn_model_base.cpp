//
// Created by HP on 2026/11/3.
//
#include "faa_core/fdnn/model/base/faa_dnn_model_base.h"
using faa::core::fdnn::model::base::FaaDnnModelBase;

void FaaDnnModelBase::setConfig(const nlohmann::json& config) {
    this ->config = config;

}
void FaaDnnModelBase::setLogger(info::log::Ilog* logger) {
    this ->logger = logger;

}

bool FaaDnnModelBase::checkBackend(backend::FaaDnnBackEndType type) const {
    return type.index() == this->type.index();
}

void FaaDnnModelBase::setBackend(const std::string& type)
{
    auto it = backend::FaaDnnBackEndTypeMap.find(type);
    if (it != backend::FaaDnnBackEndTypeMap.end()){
        this->type = it->second;
    }else{
        throw std::runtime_error("backend type not found");
    }
}

void FaaDnnModelBase::printInfo()
{
    std::visit([this]<typename T0>(T0&& tag) {

         using T = std::decay_t<T0>;
         if constexpr (std::is_same_v<T, backend::kTensorRT>){
             this->backend_0.getEngineInfo();
         }
         if constexpr (std::is_same_v<T, backend::kOnnxRT>) {
             if (logger) logger->info("the backend is onnxRT, but is not implemented");
             throw std::runtime_error("ONNX backend not implemented");
         }

    }, this->type);
}

void FaaDnnModelBase::init() {

    std::visit([this]<typename T0>(T0&& tag) {

         using T = std::decay_t<T0>;
         if constexpr (std::is_same_v<T, backend::kTensorRT>){
             if (logger) logger->debug("the backend is init to  tensorRT config");
             this->backend_0.setLogger (this->logger);
             this->backend_0.setConfig (this->config);
             this->backend_0.init();
         }
         if constexpr (std::is_same_v<T, backend::kOnnxRT>) {
             if (logger) logger->info("the backend is onnxRT, but is not implemented");
             throw std::runtime_error("ONNX backend not implemented");
         }

    }, this->type);
}
