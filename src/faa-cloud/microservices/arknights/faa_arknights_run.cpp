//
// Created by HP on 2026/8/26.
//
#include "faa_cloud/microservices/arknights/service/faa_cloud_arknights_service.h"

using faa::cloud::microservices::arknigths::FaaArknightsService;
using faa::cloud::net_work::FaaCloudNetwork;

int main(){
    SetConsoleOutputCP(CP_UTF8);

    std::ifstream config_file("./config/ark/service_config.json");
    if (!config_file.is_open()) {
        std::cerr << "cannot open service_config.json\n";
        return -1;
    }
    std::ifstream config_yolo_list("./config/ark/servicer_yolo_config.json");
    if (!config_yolo_list.is_open()){
        std::cerr << "cannot open servicer_yolo_config.json\n";
        return -1;
    }

    nlohmann::json config = nlohmann::json::parse(config_file);
    nlohmann::json yolo_list = nlohmann::json::parse(config_yolo_list);

    config["yolo_list_config"]=yolo_list;

    config["yolo_ui_det_config"]["path"]="./config/yolo/trt_ocr_yolo_det.json";
    config["yolo_ui_det_config"]["entrance_layer"]="images";
    config["yolo_ui_det_config"]["exit_layer"]    ="output0";

    config["yolo_ui_det_config"]["class_labs"] = nlohmann::json::array({
        "setting",
        "announcement",
        "mail",
        "check_in",
        "sanity",
        "originium",
        "synthetic_jade",
        "lungmen_dollar",
        "level",
        "terminal",
        "formation",
        "operator",
        "shop",
        "recruitment",
        "headhunt",
        "task",
        "infrastructure",
        "warehouse",
        "friend",
        "archives",
        "activity_banner"
    });

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
    log.debug("config: {}",config.dump(2));
    service.setConfig(config);
    service.
        setNetwork(
            std::make_unique<FaaCloudNetwork>(FaaCloudNetwork(log).
                init(config)
                )
        ).
        initThreadPool(std::make_unique<faa::core::thread_pool::FAAThreadPool>()).
        setOperatorV2(log).
        setAsyncOperator(log);

    service
        .initYoloList(log)
        .initOcr(log);


    try {
        service.startV2(log);
    }catch (const std::exception& e){
        log.error("startV2 error : {}",e.what());
        return 1;
    }

    if (!service.isInitialized()){
        log.error("service is not initialized");
        return 1;
    }
    log.info("service is initialized");
    service.loop(log);
    return 0;
}