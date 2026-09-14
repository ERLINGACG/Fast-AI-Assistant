//
// Created by HP on 2026/8/26.
//
#include "faa_cloud/microservices/arknights/service/faa_cloud_arknights_service.h"

using faa::cloud::microservices::arknigths::FaaArknightsService;
using faa::cloud::net_work::FaaCloudNetwork;

int main(){
    SetConsoleOutputCP(CP_UTF8);

    nlohmann::json config;
    config["core_service_ip"]="127.0.0.1";
    config["core_service_port"]=8080;
    config["ip"]="127.0.0.1";
    config["port"]=8081;
    config["target"]="明日方舟";

    config["det_ocr_config"]["path"]="./config/ocr/trt_ocr_det.json";
    config["det_ocr_config"]["entrance_layer"]="x";
    config["det_ocr_config"]["exit_layer"]    ="fetch_name_0";

    config["rec_ocr_config"]["path"]    ="./config/ocr/trt_ocr_rec.json";
    config["rec_ocr_config"]["voc_path"]="./config/ocr/voc/ppocr_keys_v1.txt";
    config["rec_ocr_config"]["entrance_layer"]="x";
    config["rec_ocr_config"]["exit_layer"]    ="fetch_name_0";



    config["ms_info"]=nlohmann::json{
        {"service_name","arknights"},
        {"service_type","arknights"},
        {"route_operation","register"},
    };
    auto log=info::log::Ilog([&](){
        return info::log::IlogInit{
            true,
            true,
            true,
            true,
            true
        };
    });
    FaaArknightsService service;
    service.setConfig(config);
    service.
        setNetwork(std::make_unique<FaaCloudNetwork>(FaaCloudNetwork(log).init(config))).
        initThreadPool(std::make_unique<faa::core::thread_pool::FAAThreadPool>()).
        setOperator(log).
        setAsyncOperator(log);

    service.initOcr(log);

    service.start(log);

    if (!service.isInitialized()){
        log.error("service is initialized");
        return 1;
    }
    log.info("service is initialized");
    service.loop(log);
    return 0;
}