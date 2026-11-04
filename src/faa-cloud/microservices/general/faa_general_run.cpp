//
// Created by HP on 2026/10/9.
//

#include <fstream>

#include "faa_cloud/command/faa_command_tree.h"
#include "faa_cloud/microservices/general/service/faa_cloud_general.h"
using faa::cloud::microservices::general::FaaCloudGeneralServices;
using faa::cloud::microservices::base::resource::ThreadsMap;
using faa::cloud::net_work::FaaCloudNetwork;
using faa::cloud::command::FaaCommandTreeNode;
using faa::cloud::command::FaaCommandTree;

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);         // 输入 UTF-8
    auto log = info::log::Ilog([&]()
    {
        return info::log::IlogInit{
            true,
            true,
            true,
            true,
            true
        };
    });

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

    std::ifstream img_res_config("./config/resource/servicer_img_res_config.json");
    if (!img_res_config.is_open()){
        std::cerr << "cannot open servicer_img_res_config.json\n";
        return -1;
    }

    auto config = nlohmann::json::object();
    auto img_res= nlohmann::json::object();
    auto yolo_list= nlohmann::json::object();
    config_file      >> config;
    img_res_config   >> img_res;
    config_yolo_list >> yolo_list;

    config["yolo_list_config"]=yolo_list;


    FaaCloudGeneralServices services;
    services.
        setConfig(config, &log).
        setResourceConfig(img_res, &log).
        // setNetwork(
        //     std::make_unique<FaaCloudNetwork>(FaaCloudNetwork(log).
        //         init(config)
        //     )
        // ).
        registerDefine(&log).
        init();


        services.setLogger(&log);
        services.initTensorOcr(&log);
        services.initYolo(&log);
        services.initSurfFeature(&log);


        services.addCommand(&log);


        services.addAllThread(&log);
        services.addAllOper(&log);

        std::cout<<"start services"<<std::endl;
        services.start(&log);


    return 0;
}
