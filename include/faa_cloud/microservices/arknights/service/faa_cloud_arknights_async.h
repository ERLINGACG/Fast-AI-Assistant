//
// Created by HP on 2026/9/1.
//

#ifndef FASTSCREEN_FAA_CLOUD_ARKNIGHTS_ASYNC_H
#define FASTSCREEN_FAA_CLOUD_ARKNIGHTS_ASYNC_H
#include <memory>
#include <mutex>
#include <queue>
#include <nlohmann/json.hpp>
#include <string>

namespace faa::cloud::microservices::arknigths{




    // struct AsyncTask_{
    //     mutable int32_t id=0;
    //     std::string status="ready";
    //
    //     nlohmann::json requires_info{};
    //     nlohmann::json result{};
    //
    //     auto getResultDump() const{
    //         return result.dump();
    //     }
    //
    //     auto setResult(const nlohmann::json& result){
    //         this->result=result;
    //     }
    //
    //     auto getAsyncTaskStatus() const{
    //         nlohmann::json status_;
    //         status_["id"]=id;
    //         status_["status"]=status;
    //         return status_.dump();
    //     }
    // };
    //
    //
    // struct FaaArknightsAsyncTasks{
    //
    //     std::atomic<int32_t> ids;
    //     std::map  <int32_t,AsyncTask_> tasks;
    //     std::mutex mutex;
    //
    //     template<typename F>
    //     auto addTask(const AsyncTask_ & task,F&& f){
    //         std::lock_guard lock(mutex);
    //         task.id=ids.load();
    //         tasks[task.id]=task;
    //         ++ids;
    //         f();
    //         return task.id;
    //     }
    //
    //     auto setTaskStatus(const int32_t id, const std::string& status){
    //         std::lock_guard lock(mutex);
    //         if (tasks.contains(id)){
    //             tasks[id].status=status;
    //         }
    //     }
    //
    //     auto setTaskResult(const int32_t id, const nlohmann::json& result){
    //         std::lock_guard lock(mutex);
    //         if (tasks.contains(id)){
    //             tasks[id].setResult(result);
    //         }
    //     }
    //
    //     auto getTask(const int32_t id){
    //         std::lock_guard lock(mutex);
    //         if (tasks.contains(id)){
    //             auto task=tasks[id];
    //             if(task.status=="over"){
    //                 tasks.erase(id);
    //             }
    //             return task;
    //         }
    //         return AsyncTask_{
    //             .id=-1,
    //             .status="unknown",
    //         };
    //     }
    //
    // };

}
#endif //FASTSCREEN_FAA_CLOUD_ARKNIGHTS_ASYNC_H
