//
// Created by HP on 2026/9/1.
//
#define WIN32_LEAN_AND_MEAN
#include "faa_core/win_utils/faa_win_utils.h"
#include "faa_cloud/common/ocr/faa_cloud_ocr_oper.h"
#include "faa_cloud/microservices/arknights/service/faa_cloud_arknights_service.h"
#include "faa_cloud/network/faa_cloud_network_ack.h"

using faa::cloud::microservices::arknigths::FaaArknightsService;

FaaArknightsService& FaaArknightsService::setAsyncOperator(info::log::Ilog& log){




    addOcrAsyncOperatorV2(log);

    return *this;
}

void FaaArknightsService::addOcrAsyncOperatorV2(info::log::Ilog& log) {

    this->operator_map_v2["async_ocr"]=[this,&log](net_work::FaaCloudNetworkLoad& load,sockaddr_in& response_addr){

        auto task= common::async::AsyncTask();
        auto operator_info=load.body().data;
        this->async_tasks.addTask(
           task,[this,&response_addr,&task, &load](){

                load.setBody(task.dump());

                this->network_->sendLoad(load,response_addr);
        });
        log.info("async_ocr operator_info:{}",operator_info.dump());
        load.setBody(operator_info);
        const auto start=std::chrono::high_resolution_clock::now();

        common::ocr::FaaCloudOcrAck ocr_ack{};
        this->ocrOperator(log,load,ocr_ack);

        const auto end = std::chrono::high_resolution_clock::now();
        log.info("ocr cost:{}", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

        ocr_ack.data["cost"] = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        this->async_tasks.setTaskOver(task.id,ocr_ack.data);


    };
}


void FaaArknightsService::addGetTask(info::log::Ilog& log){
    this->operator_map_v2["get_task"]=[this,&log](net_work::FaaCloudNetworkLoad& load,sockaddr_in& response_addr) {
        
        auto request_info = load.body().data;  
        auto details = this -> async_tasks.getTask(request_info.value("task_ids",0));
        load.setBody(details.dump());
        this->network_->sendLoad(load,response_addr);

    };
}