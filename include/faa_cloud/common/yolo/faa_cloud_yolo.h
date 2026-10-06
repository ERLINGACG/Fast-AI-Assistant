//
// Created by HP on 2026/10/3.
//

#ifndef FASTSCREEN_FAA_CLOUD_YOLO_H
#define FASTSCREEN_FAA_CLOUD_YOLO_H
#include <memory>

#include "faa-core/fdnn/runtime/faa_dnn_executor.h"

namespace faa::cloud::common::yolo {


    template <typename B,typename C,typename P>
    struct FaaCloudYolo {

        using exc=std::unique_ptr<fdnn::rt::FaaDnnExecutor<B,C>>;
        using proc=std::unique_ptr<P>;

        std::mutex lock;
        exc  executor;
        proc process;
        C    ctx;

        nlohmann::json config;

        std::string entrance_type{};
        std::string entrance_layer{};
        std::string exit_layer{};
        std::string exit_type{};

        auto& setConfig(const nlohmann::json& config){

            this->config=config;
            this->entrance_type =config.value("entrance_type","fp32");
            this->entrance_layer=config.value("entrance_layer","");
            this->exit_type     =config.value("exit_type","fp32");
            this->exit_layer    =config.value("exit_layer","");

            return *this;
        }

        auto& setProcess(std::unique_ptr<P> process){
            this->process = std::move(process);
            return *this;
        }


        auto& setExecutor(std::unique_ptr<fdnn::rt::FaaDnnExecutor<B,C>> executor){
            this->executor = std::move(executor);
            return *this;
        }

    };
}
#endif //FASTSCREEN_FAA_CLOUD_YOLO_H