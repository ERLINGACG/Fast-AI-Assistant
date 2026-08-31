//
// Created by HP on 2026/8/5.
//

#ifndef FASTSCREEN_FAA_SERVICE_RESOURCES_THREAD_H
#define FASTSCREEN_FAA_SERVICE_RESOURCES_THREAD_H
#include <memory>
#include <thread>

#include "faa-core/thread_pool/faa_thread_pool.h"

namespace faa::service::resources::thread{
    struct Threads{
        std::unique_ptr<std::thread> udp_thread=nullptr;
        std::unique_ptr<std::thread> cmd_thread=nullptr;
        std::unique_ptr<std::thread> lua_thread=nullptr;
    };

    struct ThreadPools{
        int32_t short_task_pool_size{1};
        int32_t long_task_pool_size{1};

        std::unique_ptr<core::thread_pool::FAAThreadPool> short_task=nullptr;
        std::unique_ptr<core::thread_pool::FAAThreadPool> long_task =nullptr;

        bool init(){
            short_task=std::make_unique<core::thread_pool::FAAThreadPool>(short_task_pool_size);
            long_task =std::make_unique<core::thread_pool::FAAThreadPool>(long_task_pool_size);
            return short_task!=nullptr && long_task!=nullptr;
        }
    };
}
#endif //FASTSCREEN_FAA_SERVICE_RESOURCES_THREAD_H