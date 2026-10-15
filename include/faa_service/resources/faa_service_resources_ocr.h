//
// Created by HP on 2026/8/5.
//

#ifndef FASTSCREEN_FAA_SERVICE_RESOURCES_OCR_H
#define FASTSCREEN_FAA_SERVICE_RESOURCES_OCR_H
#include <cstdint>
#include <string>

#include "faa_service_resources_dnn.h"
#include "faa_core/fdnn/fcv/faa_dnn_cv_trt.h"
#include "faa_core/fdnn/fcv/processer/ocr/faa_ocr_det_proc.h"
#include "faa_core/fdnn/fcv/processer/ocr/faa_ocr_rec_proc.h"
#include "faa_core/fdnn/runtime/faa_dnn_executor.h"

namespace faa::service::resources::ocr{


    struct FaaOcrRuntime{
        using FaaCvCtx=fdnn::fcv::FaaCvCtx;
        using FaaOcrDetProc=fdnn::fcv::processer::ocr::FaaOcrDetProc;
        using FaaOcrRecProc=fdnn::fcv::processer::ocr::FaaOcrRecProc;
        using FaaCvDnnForTensorRT=fdnn::fcv::FaaCvDnnForTensorRT;


        std::unique_ptr<dnn::FaaServiceCvDnnV2<FaaCvDnnForTensorRT, FaaCvCtx, FaaOcrDetProc>> det_dnn=std::make_unique<dnn::FaaServiceCvDnnV2<FaaCvDnnForTensorRT, FaaCvCtx, FaaOcrDetProc>>();
        std::unique_ptr<dnn::FaaServiceCvDnnV2<FaaCvDnnForTensorRT, FaaCvCtx, FaaOcrRecProc>> rec_dnn=std::make_unique<dnn::FaaServiceCvDnnV2<FaaCvDnnForTensorRT, FaaCvCtx, FaaOcrRecProc>>();
        void detInit(nlohmann::json& config) const;
        void recInit(nlohmann::json& config) const;
    };

    struct FaaOcrDnnResource {
        using FaaCvCtx=fdnn::fcv::FaaCvCtx;
        using FaaOcrDetProc=fdnn::fcv::processer::ocr::FaaOcrDetProc;
        using FaaCvDnnForTensorRT=fdnn::fcv::FaaCvDnnForTensorRT;

        std::mutex mtx;
        std::map<std::string,FaaOcrRuntime> ocr_resources{};

        auto push(const std::string& global_name,nlohmann::json& config){
            if (!ocr_resources.contains(global_name)){
                ocr_resources[global_name].detInit(config);
                ocr_resources[global_name].recInit(config);
            }
        }

        auto& peek(const std::string& global_name,nlohmann::json& config){
            std::lock_guard lock(mtx);
            if (!ocr_resources.contains(global_name)){
                this->push(global_name,config);
            }
            return ocr_resources[global_name];
        }

        auto& peek(const std::string& global_name){
            std::lock_guard lock(mtx);
            return ocr_resources[global_name];
        }


    };

}
#endif //FASTSCREEN_FAA_SERVICE_RESOURCES_OCR_H