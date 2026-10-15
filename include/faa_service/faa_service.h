//
// Created by HP on 2026/6/29.
//

#ifndef FASTSCREEN_FS_SERVICE_RUN_H
#define FASTSCREEN_FS_SERVICE_RUN_H
#include <map>
#include <thread>
#include <utility>
#include <nlohmann/json.hpp>

#include "net_work/faa_net_work.h"
#include "runtime/faa_service_runtime.h"
#include "faa_core/faa_data.h"
#include "../faa_core/thread_pool/faa_thread_pool.h"
#include "faa_core/input/windows_input.h"
#include "faa_core/wgc_cap/faa_wgc_cap.h"
#include "info/ilog/ilog.h"

namespace faa::service {

    struct ModelResourceConfig{
        std::string ocr_det_path;
        std::string ocr_rec_path;
        nlohmann::json dnn_layers;
    };

    class FAAServiceUDP {

            nlohmann::json resources_config_json;

            ModelResourceConfig model_resource_config;

            std::thread loop_thread;   // UDP 接收线程
            std::thread cmd_thread;    // 命令行控制线程
            std::unique_ptr<core::thread_pool::FAAThreadPool> thread_pool;

            net_work::NetWorkConfig config{};
            net_work::NetWorkUDPRuntime runtime{};
            info::log::Ilog log;

            std::atomic<bool> is_running{true};

            std::map<std::string,std::unique_ptr<rt::FaaServiceOcrResources>>     ocr_resources;
            std::map<std::string,std::unique_ptr<input::windows::WindowsInput>>   win32_inputs;


            std::map<std::string,std::vector<cv::Mat>> frames_map{};
            std::mutex window_capture_mutex;
            std::mutex frame_mutex;

        public:
          template<typename T>
          FAAServiceUDP(T config_func,info::log::Ilog log): config(config_func()), log(log){
              log.info("max_threads: {}",config.max_threads);
              thread_pool=std::make_unique<core::thread_pool::FAAThreadPool>(config.max_threads);
          }
          int  resource_config(std::string config_path);
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