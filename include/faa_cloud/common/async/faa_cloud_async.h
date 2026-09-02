//
// Created by HP on 2026/9/2.
//

#ifndef FASTSCREEN_FAA_CLOUD_ASYNC_H
#define FASTSCREEN_FAA_CLOUD_ASYNC_H
#include <mutex>
#include <nlohmann/json.hpp>

namespace faa::cloud::common::async{

    struct AsyncTask{
        int32_t id{};
        std::string status;
        nlohmann::json result{};

        auto setResult(const nlohmann::json& result){
            this->result["result"]=result;
        }

        [[nodiscard]] auto dump() {
            result["id"]=id;
            result["status"]=status;
            return result.dump();
        }
    };

    struct AsyncTasks{
        std::atomic<int32_t> id=0;
        std::mutex mutex;
        std::map<int32_t,AsyncTask> tasks{};


        template<typename F>
        auto addTask(AsyncTask& task,F&& callback){
            std::lock_guard lock(mutex);
            task.status="started";
            tasks[id.fetch_add(1)]=task;
            callback();
        }

        auto setTaskOver(const int32_t id, const nlohmann::json& result){
            std::lock_guard lock(mutex);
            if (tasks.contains(id)){
                tasks[id].status="over";
                tasks[id].setResult(result);
            }
        }

        auto setTaskStatus(const int32_t id, const std::string& status){
            std::lock_guard lock(mutex);
            if (tasks.contains(id)){
                tasks[id].status=status;
            }
        }

        auto setTaskResult(const int32_t id, const nlohmann::json& result){
            std::lock_guard lock(mutex);
            if (tasks.contains(id)){
                tasks[id].setResult(result);
            }
        }

        auto getTask(const int32_t id){
            std::lock_guard lock(mutex);
            if (tasks.contains(id)){
                const auto task=tasks[id];
                if (task.status=="over"){
                    tasks.erase(id);
                }
                return task;
            }
            return AsyncTask{
                .id=-1,
                .status="unknown",
                .result=nlohmann::json(),
            };
        }

    };




}
#endif //FASTSCREEN_FAA_CLOUD_ASYNC_H