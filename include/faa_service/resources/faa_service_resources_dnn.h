//
// Created by HP on 2026/8/7.
//

#ifndef FASTSCREEN_FAA_SERVICE_RESOURCES_DNN_H
#define FASTSCREEN_FAA_SERVICE_RESOURCES_DNN_H
#include <memory>
#include <string>

#include "faa-core/fdnn/runtime/faa_dnn_executor.h"

namespace faa::service::resources::dnn {

    inline  std::map<std::string,gdlz::trt::core::cuda::DataType> types_mapper{
              {"fp32",gdlz::trt::core::cuda::DataType::FLOAT32},
              {"fp16",gdlz::trt::core::cuda::DataType::FLOAT16},
              {"int8",gdlz::trt::core::cuda::DataType::INT8},
    };

    template<typename B,typename C,typename P>
    struct FaaServiceCvDnnV2{

        std::string global_name;
        int32_t width{};
        int32_t height{};
        std::string entrance_type{};
        std::string entrance_layer{};
        std::string exit_layer{};
        std::string exit_type{};
        std::unique_ptr<fdnn::rt::FaaDnnExecutor<B,C>> executor;
        std::unique_ptr<P> proc;
        std::unique_ptr<C> ctx;

        auto contains(const std::string& global_name){
            return global_name==this->global_name;
        }

        auto initMeta(nlohmann::json persistence){
            this->width          = persistence["width"].get<int32_t>();
            this->height         = persistence["height"].get<int32_t>();
            this->entrance_type  = persistence["entrance"]["type"].get<std::string>();
            this->entrance_layer = persistence["entrance"]["name"].get<std::string>();
            this->exit_type      = persistence["exit"]["type"].get<std::string>();
            this->exit_layer     = persistence["exit"]["name"].get<std::string>();
        }

        template<typename... Args>
        auto initRt(Args&&... args){
            ctx=std::make_unique<C>();
            executor = std::make_unique<fdnn::rt::FaaDnnExecutor<B,C>>(std::make_unique<B>(std::forward<Args>(args)...));
            executor->initEngine();
            executor->initCtx(*this->ctx);

        }

        auto input(const std::string& name, const std::vector<int64_t>& dims){
            executor->setInput(name,dims,*ctx);
        }
        auto output(const std::string& name,const std::string& type) {
            executor->setOutput(name,types_mapper[type],*ctx);
        }
    };
}
#endif //FASTSCREEN_FAA_SERVICE_RESOURCES_DNN_H