//
// Created by HP on 2026/11/3.
//
#include "faa_core/fdnn/model/base/faa_dnn_model_base.h"

#include "faa_core/fdnn/backend/faa_backend_type_oper.h"
using faa::core::fdnn::model::base::FaaDnnModelBase;

void FaaDnnModelBase::setConfig(const nlohmann::json& config) {
    this ->config = config;
}

// void FaaDnnModelBase::setLogger(info::log::Ilog* logger) {
//     this ->logger = logger;
// }

void FaaDnnModelBase::setLogger(common::ilogger::FaaIlogger* ilogger) {
    this ->ilogger = ilogger;
}


void FaaDnnModelBase::shapeInit(FaaDnnModelBase& provider)
{
    backend::visit_to_type(
        this->type,
        [this, &provider]() {
            this->getBackend0().initBackend(
                provider.getBackend0().getBackend()
            );

            this->initCtx();
        },
        [this]() {
            if (ilogger) ilogger->info("the backend is onnxRT, but is not implemented");
            throw std::runtime_error("ONNX backend not implemented");
        }
    );
}
bool FaaDnnModelBase::checkBackend(backend::FaaDnnBackEndType type) const {
    return type.index() == this->type.index();
}

void FaaDnnModelBase::setBackend(const std::string& type) {
    auto it = backend::FaaDnnBackEndTypeMap.find(type);
    if (it != backend::FaaDnnBackEndTypeMap.end()){
        this->type = it->second;
    }else{
        throw std::runtime_error("backend type not found");
    }
}

void FaaDnnModelBase::printInfo() {

    backend::visit_to_type(
    this->type,
        [this](){
            this->backend_0.getEngineInfo();
        },
        [this]() {
            if (ilogger) ilogger->info("the backend is onnxRT, but is not implemented");
            throw std::runtime_error("ONNX backend not implemented");
        }
    );
}

void FaaDnnModelBase::initBackend()
{

    backend::visit_to_type(
        this->type,
        [this](){
            this->backend_0.setLogger (this->ilogger);
            this->backend_0.setConfig (this->config);
            this->backend_0.initBackend();
        },
        [this]() {
            if (ilogger) ilogger->info("the backend is onnxRT, but is not implemented");
            throw std::runtime_error("ONNX backend not implemented");
        }
    );

}

void FaaDnnModelBase::initCtx() {


    backend::visit_to_type(
        this->type,
        [this](){
            this->backend_0.initCtx();
        },
        [this]() {
            if (ilogger) ilogger->info("the backend is onnxRT, but is not implemented");
            throw std::runtime_error("ONNX backend not implemented");
        }
    );
}
