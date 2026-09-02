//
// Created by HP on 2026/9/1.
//
#define WIN32_LEAN_AND_MEAN
#include "faa-core/win_utils/faa_win_utils.h"
#include "faa_cloud/microservices/arknights/service/faa_cloud_arknights_service.h"
using faa::cloud::microservices::arknigths::FaaArknightsAsyncTasks;
using faa::cloud::microservices::arknigths::FaaArknightsService;

FaaArknightsService& FaaArknightsService::setAsyncOperator(info::log::Ilog& log){

    auto get_async_task=[this,&log](const nlohmann::json& requires_info,sockaddr_in& response_addr){
        auto id=requires_info.value("id",0);
        auto task=this->async_tasks.getTask(id);
        this->network_->sendMessage(task.dump(),response_addr);
    };

    auto async_sleep=[this,&log](const nlohmann::json& requires_info,sockaddr_in& response_addr){
       auto task= common::async::AsyncTask();

       this->async_tasks.addTask(task,[this,&response_addr,&task](){
                this->network_->sendMessage("async_sleep+id:"+std::to_string(task.id),response_addr);
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

         if (!this->win32input->isInit()){
             auto hwnd=core::win_utils::GetWindowHandle(this->config_.value("target","") );
             if (hwnd==nullptr){
                hwnd=core::win_utils::GetWindowHandleByClass(this->config_.value("target",""));
             }
             if (hwnd==nullptr){
                 hwnd=core::win_utils::GetWindowHandleByName(this->config_.value("target",""));
             }
             if (hwnd==nullptr){
                log.error("get window handle failed");
                return;
             }
             log.debug("hwnd is found");
             this->win32input->set(
                 std::make_unique<input::windows::WindowsInput>(hwnd)
             );
         }
         for(auto& it:requires_info.value("inputs",nlohmann::json::array())){
             if (auto type_name=it.value("type",""); this->win32input->operators.contains(type_name)){
                this->win32input->operators[type_name](it);
             }
         }

        this->async_tasks.setTaskOver(task.id,nlohmann::json({}));

    };

    this->operator_map["async_sleep"]=async_sleep;
    this->operator_map["get_async_task"]=get_async_task;
    this->operator_map["async_input"]=async_input;

    return *this;
}