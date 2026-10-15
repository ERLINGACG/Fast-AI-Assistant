//
// Created by HP on 2026/10/13.
//
#include "faa_cloud/microservices/general/service/faa_cloud_general.h"
using faa::cloud::microservices::general::FaaCloudGeneralServices;

void FaaCloudGeneralServices::initTensorOcr(info::log::Ilog* log) {

    if (log)log->debug(
        "initOcr det:{}",this->config.value("det_ocr_config",nlohmann::json({})
            ).dump()
    );
    if (log)log->debug(
        "initOcr rec:{}",this->config.value("rec_ocr_config",nlohmann::json({})
            ).dump()
    );
    auto det_config = this->config.value("det_ocr_config",nlohmann::json({}));
    auto rec_config = this->config.value("rec_ocr_config",nlohmann::json({}));

    if (det_config.empty()){
       if (log) log->error("det_ocr_config is not found");
        return;
    }
    if (rec_config.empty()){
       if (log) log->error("rec_ocr_config is not found");
        return;
    }
    this->tensor_ocr_pair.first = std::make_unique<
            common::ocr::FaaCloudOcr<
                TensorRTBackend,
                TensorRTBackendCtx,
                OcrDetProc
            >
        > ();

    this->tensor_ocr_pair.second = std::make_unique<
            common::ocr::FaaCloudOcr<
                TensorRTBackend,
                TensorRTBackendCtx,
                OcrRecProc
            >
        > ();

    auto det=std::make_unique<
        fdnn::rt::FaaDnnExecutor<fdnn::fcv::FaaCvDnnForTensorRT,fdnn::fcv::FaaCvCtx>>(
            std::make_unique<fdnn::fcv::FaaCvDnnForTensorRT>(
                det_config["path"].get<std::string>()
                )
        );
    det->
       initEngine().
       initCtx(this->tensor_ocr_pair.first->ctx);


    auto rec=std::make_unique<
        fdnn::rt::FaaDnnExecutor<fdnn::fcv::FaaCvDnnForTensorRT,fdnn::fcv::FaaCvCtx>>(
            std::make_unique<fdnn::fcv::FaaCvDnnForTensorRT>(
                rec_config["path"].get<std::string>()
                )
        );

    rec->
       initEngine().
       initCtx(this->tensor_ocr_pair.second->ctx);



    this->tensor_ocr_pair.first->
           setConfig(det_config).
           setExecutor(std::move(det)).
           setProcess (std::make_unique<fdnn::fcv::processer::ocr::FaaOcrDetProc>());

    this->tensor_ocr_pair.second->
           setConfig(rec_config).
           setExecutor(std::move(rec)).
           setProcess (std::make_unique<fdnn::fcv::processer::ocr::FaaOcrRecProc>(
                rec_config["voc_path"].get<std::string>()
           ));



}


