//
// Created by HP on 2026/10/13.
//
#include "faa_cloud/common/yolo/faa_cloud_yolo_oper.h"
#include "faa_cloud/microservices/general/service/faa_cloud_general.h"
// #include "faa_core/fdnn/fcv/faa_dnn_cv_trt.h"
using faa::cloud::microservices::general::FaaCloudGeneralServices;

void FaaCloudGeneralServices::addYoloOper(info::log::Ilog* log) {
    this->operat.push("yolo",[this,log]
    (net_work::FaaCloudNetworkLoad& load,sockaddr_in response_addr){

        log->debug("yolo to {}",load.unserialize().dump());
        auto param = load.body().data;
        cv::Mat frame;
        this->cap.frame_resources.peek(frame);

        if (frame.empty()) {
            log->error("frame is empty");
            load.setBody({
                {"error","frame is empty"}
            });
            this->network.get().sendLoad(load,response_addr);
            return 0;
        }

        const auto model_name = param.value("model_name","");
        if (model_name.empty()) {
            load.setBody({
                {"error","model_name is empty"}
            });
            this->network.get().sendLoad(load,response_addr);
            return 0;
        }

        const auto model = this->yolo_map.find(model_name);
        if (model == this->yolo_map.end()) {
            load.setBody({
                {"error","model_name is not found"}
            });
            this->network.get().sendLoad(load,response_addr);
            return 0;
        }

        const auto& yolo = model->second;
        fdnn::fcv::FaaCvOutputData output{};
        fdnn::fcv::processer::ob::FaaYoloResult result{};
        common::yolo::yoloOper(param,*yolo,frame,output,result);

        load.setBody({
            {"result",result.serialize()}
        });
        this->network.get().sendLoad(load,response_addr);

        return 0;
    });
}

void FaaCloudGeneralServices::addGetYoloModel(info::log::Ilog* log) {

    this->operat.push("get_yolo_model",[this,log]
    (net_work::FaaCloudNetworkLoad& load,sockaddr_in response_addr){

        log->debug("get_yolo_model to {}",load.unserialize().dump());

        auto model_array = nlohmann::json::array();
        for (const auto& name : this->yolo_map | std::views::keys){
            model_array.push_back({
                {"name",name}
            });
        }
        load.setBody({
            {"model",model_array}
        });
        this->network.get().sendLoad(load,response_addr);


        return 0;
    });
}
