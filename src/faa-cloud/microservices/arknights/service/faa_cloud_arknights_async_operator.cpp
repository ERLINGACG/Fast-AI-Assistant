//
// Created by HP on 2026/9/1.
//
#define WIN32_LEAN_AND_MEAN
#include "faa-core/win_utils/faa_win_utils.h"
#include "faa_cloud/common/ocr/faa_cloud_ocr_oper.h"
#include "faa_cloud/microservices/arknights/service/faa_cloud_arknights_service.h"
#include "faa_cloud/network/faa_cloud_network_ack.h"
using faa::cloud::microservices::arknigths::FaaArknightsAsyncTasks;
using faa::cloud::microservices::arknigths::FaaArknightsService;

FaaArknightsService& FaaArknightsService::setAsyncOperator(info::log::Ilog& log){



    auto async_sleep=[this,&log](const nlohmann::json& requires_info,sockaddr_in& response_addr){
       auto task= common::async::AsyncTask();

       this->async_tasks.addTask(task,[this,&response_addr,&task](){
                this->network_->sendMessage(task.dump(),response_addr);
        });

        const int32_t sleep=requires_info.value("sleep",0);
        log.info("sleep:{}",sleep);

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));

        this->async_tasks.setTaskOver(task.id,nlohmann::json({}));
    };


    auto async_input=[this,&log](const nlohmann::json& requires_info,sockaddr_in& response_addr){

        auto task= common::async::AsyncTask();

        this->async_tasks.addTask(task,[this,&response_addr,&task](){
                 this->network_->sendMessage("async_sleep+id:"+std::to_string(task.id),response_addr);
         });

        this->win32InputOperator(log,requires_info);

        this->async_tasks.setTaskOver(task.id,nlohmann::json({}));

    };



    this->operator_map["async_sleep"]=async_sleep;
    this->operator_map["async_input"]=async_input;



    addGetAsyncTaskOperator(log);
    addOcrAsyncOperator(log);

    return *this;
}

void FaaArknightsService::addGetAsyncTaskOperator(info::log::Ilog& log){

    this->operator_map["get_async_task"]=[this,&log](
        const nlohmann::json& requires_info, const sockaddr_in& response_addr
        ){
            auto id=requires_info.value("id",0);
            auto task=this->async_tasks.getTask(id);

            auto ack=net_work::FaaCloudNetworkAck();

            ack.unserialize("success",nlohmann::json({
                {"task",task.dump()}
                })
            );
            this->network_->sendMessage(ack.serialize(),response_addr);
    };
}

void FaaArknightsService::addOcrAsyncOperator(info::log::Ilog& log) {

    this->operator_map["async_ocr"]=[this,&log](nlohmann::json& requires_info,sockaddr_in& response_addr){
        auto task= common::async::AsyncTask();
        auto start=std::chrono::high_resolution_clock::now();

         auto net_ack=net_work::FaaCloudNetworkAck();
        this->async_tasks.addTask(
            task,[this,&response_addr,&task, &net_ack](){
                 net_ack.unserialize("success",task.dump());
                 this->network_->sendMessage(net_ack.serialize(),response_addr);
        });

        common::ocr::FaaCloudOcrAck ocr_ack{};
        this->ocrOperator(log,requires_info,ocr_ack);

        auto end = std::chrono::high_resolution_clock::now();
        log.info("ocr cost:{}", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

        ocr_ack.data["cost"] = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        this->async_tasks.setTaskOver(task.id,ocr_ack.data);

    };
}


