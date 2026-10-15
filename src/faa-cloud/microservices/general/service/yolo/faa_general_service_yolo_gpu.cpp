//
// Created by HP on 2026/10/13.
//
#include "faa_cloud/microservices/general/service/faa_cloud_general.h"
using faa::cloud::microservices::general::FaaCloudGeneralServices;

void FaaCloudGeneralServices::initYolo(info::log::Ilog* log) {
    auto array = this->config.
           value("yolo_list_config",nlohmann::json({})).
           value("list",nlohmann::json::array());

    if (log) log->debug("initYoloList:{}",array.dump());

    int ids = 0;
    for (auto& item:array){

        if (log) log->debug("{}",item.dump(2));

        if (!item.contains("path")){
            if (log) log->warn("yolo_list_config item path is not found to {}",item.value("name",""));
            continue;
        }
        auto trt_executor=std::make_unique<
                   common::yolo::FaaCloudYolo<
                       fdnn::fcv::FaaCvDnnForTensorRT,
                       fdnn::fcv::FaaCvCtx,
                       fdnn::fcv::processer::ob::FaaYoloProc
                    >
        >();

        auto yolo_model=std::make_unique<fdnn::rt::FaaDnnExecutor<
            TensorRTBackend,TensorRTBackendCtx>
        >(
            std::make_unique<TensorRTBackend>(
                item["path"].get<std::string>()
            )
        );
        //
        yolo_model->
            initEngine().
            initCtx(trt_executor->ctx);

        auto yolo_proc=std::make_unique<fdnn::fcv::processer::ob::FaaYoloProc>(item);

        trt_executor->
            setExecutor(std::move(yolo_model)
            ).
            setProcess(std::move(yolo_proc)).
            setConfig(item);

        //
        this->yolo_map[item.value("name","yolo_"+std::to_string(ids))]=
            std::move(trt_executor);

        if (log) log->debug("yolo_list_config item {} init done",item.value("name",""));
        ids++;
    }
}
