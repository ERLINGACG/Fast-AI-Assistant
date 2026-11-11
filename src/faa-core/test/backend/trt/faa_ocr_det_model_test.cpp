//
// Created by HP on 2026/11/3.
//
#include "faa_core/fdnn/model/ocr/faa_ocr_det_model.h"
#include "faa_core/fdnn/model/ocr/faa_ocr_rec_model.h"
#include "faa_core/fdnn/backend/trt_cv/faa_cv_dnn_trt.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "faa_common/ilogger/faa_ilogger.h"
#include "faa_common/log/faa_logger.h"
#include "faa_core/fdnn/backend/trt_cv/data/faa_cv_dnn_trt_type.h"

using faa::core::fdnn::model::ocr::FaaOcrDetModel;
using faa::core::fdnn::model::ocr::FaaOcrRecModel;
using faa::common::ilogger::FaaIlogger;
using faa::core::fdnn::backend::trt::kFaaFloat32;
using faa::core::fdnn::backend::kFaaTensorRT;
using faa::core::fdnn::backend::kTensorRT;
using faa::core::fdnn::general::ocr::FaaDetResults;

auto logger = info::log::Ilog([&]() {
        return info::log::IlogInit{
            true,
            true,
            true,
            true,
            true
        };
    });

auto logger_v2 = FaaIlogger({});

auto test(info::log::Ilog* logger) {

    auto config =std::ifstream("./config/trt/ocr_det_config.json");
    nlohmann::json config_json = nlohmann::json::parse(config);


    auto img = faa::core::frame::FaaMat();

    if (!img.read("./test/image/img.png")){
        logger->error("load image failed");
        return;
    }

    try {
        auto result = FaaDetResults();
        auto provider = FaaOcrDetModel();
        provider.setBackend(kFaaTensorRT);
        provider.setLogger(&logger_v2);
        provider.setConfig(config_json);
        provider.initBackend();
        provider.printInfo();


        auto model = FaaOcrDetModel();
        model.setConfig(config_json);
        model.setLogger(&logger_v2);
        model.shapeInit(provider);

        for (int i = 0; i < 10; i++) {
            auto start = std::chrono::high_resolution_clock::now();
            model.run(
            img,{
                    .param = {
                        .threshold = 0.7,
                        .pad       = 4,
                        .save_bin   = false,
                    },
                    .isSave = false
                },
                result
            );
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            logger->info("run time: {}",duration.count());

        }
        model.run(
            img,{
                    .param = {
                        .threshold = 0.7,
                        .pad       = 4,
                        .save_bin   = true,
                    },
                    .isSave = true
                },
                result
            );

    }
    catch(const std::exception& e){
        logger->error("init model failed: {}",e.what());
    }


}

auto test_2() {
    try  {
        auto det_config =std::ifstream("./config/trt/ocr_det_config.json");
        auto rec_config =std::ifstream("./config/trt/ocr_rec_config.json");
        nlohmann::json det_config_json = nlohmann::json::parse(det_config);
        nlohmann::json rec_config_json = nlohmann::json::parse(rec_config);

        auto input = faa::core::frame::FaaMat();

        if (!input.read("./test/image/img_1.png")){
            logger_v2.error("load image failed");
            return;
        }

        auto det = FaaOcrDetModel();
        auto rec = FaaOcrRecModel();

        logger_v2.info("============ init det model ===============");
        det.setBackend(kFaaTensorRT);
        det.setLogger(&logger_v2);
        det.setConfig(det_config_json);
        det.initBackend();
        det.initCtx();
        det.printInfo();

        logger_v2.info("============ init rec model ===============");
        rec.setBackend(kFaaTensorRT);

        rec.setLogger(&logger_v2);
        rec.setConfig(rec_config_json);
        rec.loadVocab();
        rec.initBackend();
        rec.initCtx();
        rec.printInfo();


        int max = 1;
        for (int i = 0; i < max; i++) {

            auto det_result = FaaDetResults();
            auto rec_result = faa::core::fdnn::general::ocr::FaaRecResults();
            det_result.items.clear();
            auto det_time = std::chrono::high_resolution_clock::now();
            det.run(
            input,{
                    .param = {
                        .threshold = 0.7,
                        .pad       = 5,
                        .save_bin   = false,
                    },
                    .isSave = true
                },
                det_result
            );
            auto det_end = std::chrono::high_resolution_clock::now();
            auto det_duration = std::chrono::duration_cast<std::chrono::milliseconds>(det_end - det_time);
        //
            auto rec_param = faa::core::fdnn::general::ocr::FaaRecParam();
            rec.run(
                input,
                rec_param,
                det_result,
                rec_result
            );
            auto rec_time = std::chrono::high_resolution_clock::now();
            auto rec_duration = std::chrono::duration_cast<std::chrono::milliseconds>(rec_time - det_end);
            logger_v2.info("det run time: {} , rec run time: {}",det_duration.count(),rec_duration.count());
            if (i == max - 1) {
                logger_v2.info("============ rec result ===============");
                for (auto& item : rec_result.items) {
                    logger_v2.info(
                        "text: {} score: {} x: {} y: {} w: {} h: {}",
                        item.text,item.score,item.x,item.y,item.w,item.h);
                }
            }

        }



    }catch (
        const std::exception& e
    ){
        logger_v2.error("init model failed: {}",e.what());
    }
}


int main() {
    // test(&logger);
    test_2();
    // test3();
    return 0;
}