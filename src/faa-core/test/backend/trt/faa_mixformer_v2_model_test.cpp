//
// Created by HP on 2026/11/6.
//
#include "faa_core/fdnn/model/mixformer_v2/faa_mixformer_v2_model.h"

#include <fstream>
using faa::core::fdnn::model::mixformer_v2::FaaMixformerV2Model;
using faa::core::fdnn::model::mixformer_v2::FaaMixformerRunParam;
using faa::core::fdnn::backend::kFaaTensorRT;
using faa::common::ilogger::FaaIlogger;
auto logger_v2 = FaaIlogger({});
auto test() {
    auto mixfomer_v2_config =std::ifstream("./config/trt/mixfomer_v2_config.json");
    nlohmann::json mixfomer_v2_config_json = nlohmann::json::parse(mixfomer_v2_config);

    auto img  = faa::core::frame::FaaMat();
    auto temp = faa::core::frame::FaaMat();
    if (!img.read("./test/image/ark_1.png")){
        logger_v2.error("load image failed");
        return;
    }
    if (!temp.read("./test/temp/ioc_7.png")) {
        logger_v2.error("load temp config failed");
        return;
    }

    auto model = FaaMixformerV2Model();
    model.setBackend(kFaaTensorRT);


    model.setLogger(&logger_v2);
    model.setConfig(mixfomer_v2_config_json);
    model.initBackend();
    model.initCtx();
    model.printInfo();
    auto start = std::chrono::high_resolution_clock::now();
    model.run(img, temp,
        FaaMixformerRunParam{
            .search_x = 1000,
            .search_y = 1000,
            .search_w = 548,
            .search_h = 548,
            .step = 168,
            .score_threshold = 0.1,
            .nms_threshold = 0.1,
            .max_targets = 1,
            .enable_debug_draw = true,

        }
    );
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    logger_v2.info("run time: {} ms", duration.count());
}



int main()
{
    try {
        test();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}