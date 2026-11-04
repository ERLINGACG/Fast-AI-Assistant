//
// Created by HP on 2026/10/31.
//
#include <fstream>

#include "faa_core/fdnn/backend/trt_cv/faa_cv_dnn_trt.h"
#include "faa_core/fdnn/backend/trt_cv/data/faa_cv_dnn_trt_type.h"
#include "faa_core/fdnn/general/ocr/faa_cv_ocr_det_pros.h"
using faa::core::frame::FaaMat;
using faa::core::fdnn::backend::trt::FaaCvTensorRtBackend;
using faa::core::fdnn::backend::trt::kFaaFloat32;
auto ocr_test(info::log::Ilog* ilog)
{
    // auto config =std::ifstream("./config/trt/ocr_det_config.json");
    // //
    // nlohmann::json config_json = nlohmann::json::parse(config);
    // ilog->info("config_json: {}",config_json.dump(2));
    //
    // auto backend = FaaCvTensorRtBackend();
    // backend.setLogger(ilog);
    // backend.setConfig(config_json);
    // try {
    //
    //     auto input_img_1    = FaaMat();
    //     auto input_blob_1   = FaaMat();
    //     auto output_blob_1  = FaaMat();
    //
    //     if (!input_img_1.read("./test/image/test2.png")){
    //         ilog->error("load image failed");
    //         return;
    //     }
    //
    //     faa::core::fdnn::general::ocr::setBlob(
    //         input_blob_1,
    //         input_img_1
    //     );
    //
    //
    //     backend.init();
    //     backend.getEngineInfo();
    //     backend.setInputLayerShape ("x", input_blob_1.shape(),kFaaFloat32);
    //     backend.setOutputLayerShape("fetch_name_0", kFaaFloat32);
    //     backend.uploadBlob("x",input_blob_1);
    //
    //     for (int i = 0; i < 10; i++) {
    //         auto start = std::chrono::high_resolution_clock::now();
    //         backend.forward();
    //         auto end = std::chrono::high_resolution_clock::now();
    //         ilog->info("forward time: {}", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    //     }
    //     backend.downloadBlob("fetch_name_0",output_blob_1,kFaaFloat32);
    //     auto end = std::chrono::high_resolution_clock::now();
    //
    //     faa::core::fdnn::general::ocr::postProcess(
    //         output_blob_1,
    //         input_img_1
    //     );
    //     if (!input_img_1.write("test.png")){
    //         ilog->error("save image failed");
    //     }
    //
    // } catch (const std::exception& e) {
    //     ilog->error("init: {}", e.what());
    // }
}

int main() {

    auto logger = info::log::Ilog([&]() {
        return info::log::IlogInit{
            true,
            false,
            true,
            true,
            true
        };
    });
    ocr_test(&logger);
    // load_test(&logger);



    // backend.setLogger(&logger);
    logger.info("hello world");
    return 0;
}