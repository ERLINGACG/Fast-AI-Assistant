//
// Created by HP on 2026/11/3.
//
#include "faa_core/fdnn/model/ocr/faa_ocr_det_model.h"
#include "faa_core/fdnn/backend/trt_cv/faa_cv_dnn_trt.h"
#include "faa_core/fdnn/backend/trt_cv/faa_cv_dnn_trt_ctx.h"
#include <fstream>
#include <nlohmann/json.hpp>

#include "faa_core/fdnn/backend/trt_cv/data/faa_cv_dnn_trt_type.h"
using faa::core::fdnn::model::ocr::FaaOcrDetModel;
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
        auto model = FaaOcrDetModel();
        model.setBackend(kFaaTensorRT);
        model.setLogger(logger);
        model.setConfig(config_json);
        model.init();
        model.printInfo();
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


int main() {
    test(&logger);
    return 0;
}