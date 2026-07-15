//
// Created by HP on 2026/6/29.
//

#ifndef FASTSCREEN_FS_SERVICE_RUN_H
#define FASTSCREEN_FS_SERVICE_RUN_H
#include <map>
#include <thread>
#include <utility>

#include "faa_net_work.h"
#include "faa-core/faa_data.h"
#include "../faa-core/thread_pool/faa_thread_pool.h"
#include "faa-core/wgc_cap/faa_wgc_cap.h"
#include "info/ilog/ilog.h"

namespace faa::service {
    class FAAServiceUDP {

            std::thread loop_thread;   // UDP 接收线程
            std::thread cmd_thread;    // 命令行控制线程
            std::unique_ptr<core::thread_pool::FAAThreadPool> thread_pool;

            net_work::NetWorkConfig config{};
            net_work::NetWorkUDPRuntime runtime{};
            info::log::Ilog log;

            std::atomic<bool> is_running{true};  // 全局运行标志（替代 runtime.is_running）
            std::map<std::string,core::cap::wgc::FAAWindowCapture> window_capture;
            mutable std::mutex window_capture_mutex;

        public:
          template<typename T>
          FAAServiceUDP(T config_func,info::log::Ilog log): config(config_func()), log(log){
              log.info("max_threads: {}",config.max_threads);
              thread_pool=std::make_unique<core::thread_pool::FAAThreadPool>(config.max_threads);
          }
          int  init();


          int  run_loop();
          int  run_cmd();

          void start();
          void stop();
          void join();
          // int  stop();



    };
}
#endif //FASTSCREEN_FS_SERVICE_RUN_H