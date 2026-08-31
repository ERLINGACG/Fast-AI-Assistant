//
// Created by HP on 2026/7/28.
//

#ifndef FASTSCREEN_FAA_SERVICE_SCHEDULER_CTX_H
#define FASTSCREEN_FAA_SERVICE_SCHEDULER_CTX_H
#include <atomic>

namespace faa::service::scheduler{
    template<typename S, typename T>
    struct SchedulerCtx{

        std::atomic<bool>& is_running;
        S& service;
        T& thread_pool;

    };
}
#endif //FASTSCREEN_FAA_SERVICE_SCHEDULER_CTX_H