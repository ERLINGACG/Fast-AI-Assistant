//
// Created by HP on 2026/11/3.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_BACKEND_IMPL_H
#define FASTAIASSISTANT_ROOT_FAA_BACKEND_IMPL_H
#include "faa_core/fdnn/backend/faa_backend.h"
namespace faa::core::fdnn::backend {


    template <typename B, typename C>
    void FaaBackend<B, C>::setLogger(info::log::Ilog* logger){
        this->logger = logger;
        this->backend.setLogger(logger);
    }

    template <typename B, typename C>
    void FaaBackend<B, C>::setConfig(nlohmann::json& config){
        this->config = config;
        this->backend.setConfig(config);
    }

    template <typename B, typename C>
    void FaaBackend<B, C>::getEngineInfo() {
        backend.getEngineInfo();
    }

    template <typename B, typename C>
    void FaaBackend<B, C>::init() {
        backend.init(ctx);
    };

    template <typename B, typename C>
    template <typename... Args>
    void FaaBackend<B, C>::setInputLayerShape(std::string layer_name,std::vector<int64_t> shape , Args&&... args){
       backend.setInputLayerShape(ctx,layer_name,shape,std::forward<Args>(args)...);
    }

    template <typename B, typename C>
    template <typename ... Args>
    void FaaBackend<B, C>::setOutputLayerShape(std::string layer_name, std::string type, Args&&... args){
        backend.setOutputLayerShape(ctx,layer_name,type,std::forward<Args>(args)...);
    }


    template <typename B, typename C>
    template <typename ... Args>
    void FaaBackend<B, C>::uploadBlob(std::string layer_name, frame::FaaMat& blob, Args&&... args) {
        backend.uploadBlob(ctx,layer_name,blob,std::forward<Args>(args)...);
    }

    template <typename B, typename C>
    template <typename ... Args>
    void FaaBackend<B, C>::forward(Args&&... args) {
        backend.forward(ctx,std::forward<Args>(args)...);
    }

    template <typename B, typename C>
    template <typename ... Args>
    void FaaBackend<B, C>::downloadBlob(std::string layer_name, frame::FaaMat& output, std::string type, Args&&... args) {
        backend.downloadBlob(ctx,layer_name,output,type,std::forward<Args>(args)...);
    }
}
#endif //FASTAIASSISTANT_ROOT_FAA_BACKEND_IMPL_H