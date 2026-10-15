//
// Created by HP on 2026/7/25.
//

#ifndef FAA_FASTSCREEN_SERVICE_ORCHESTRATOR_H
#define FAA_FASTSCREEN_SERVICE_ORCHESTRATOR_H
#include "net_work/faa_service_network.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include <opencv2/core/mat.hpp>

#include "faa_service_operation.h"
#include "faa_core/input/windows_input.h"
#include "runtime/faa_servuce_runtime_init.h"

namespace faa::service::orchestrator{

    class FaaServiceOrchestrator{

        std::unique_ptr<net_work::FaaServiceUDPNetwork> network;

        std::atomic<bool> is_running{true};
        info::log::Ilog ilog;

        struct run_threads{
            std::thread udp_thread;
            std::thread cmd_thread;
            std::thread lua_thread;
        } run_threads_{};

        struct run_thread_pools{
            using thread_pool=std::unique_ptr<core::thread_pool::FAAThreadPool>;

            thread_pool ask_pool;
            thread_pool init_pool;
            thread_pool long_loop_pool;
            thread_pool proc_pool;


            auto& get_long_loop_thread_pools(){
                return long_loop_pool;
            }

            auto& get_proc_thread_pools(){
                return proc_pool;
            }

        } run_thread_pools_{};

        struct run_frame_resources {

            std::mutex frame_mutex;
            std::map<std::string,std::vector<cv::Mat>> frames;

            auto push(const std::string& key, const cv::Mat& frame,int max_frame=3){
                std::lock_guard lock(this->frame_mutex);
                frames[key].push_back(frame);
                if (frames[key].size() >= max_frame){
                    frames[key].erase(frames[key].begin());
                }
            }

            auto peek(const std::string& key,cv::Mat image){
                std::lock_guard lock(this->frame_mutex);
                if (!frames.contains(key)){
                    return;
                }
                image=frames[key].back();
            }
        } frame_resources{};

        struct run_input {
            std::mutex input_mutex;
            std::map<std::string,std::unique_ptr<input::windows::WindowsInput>>   win32_inputs{};

            auto push_win32(const std::string& key){

                if (!win32_inputs.contains(key)){
                    rt::init::win32_input_init(key, win32_inputs);
                }
            }

            auto& peek_win32(const std::string& key){
                std::lock_guard lock(input_mutex);
                if (!win32_inputs.contains(key)){
                    push_win32(key);
                    return win32_inputs[key];
                }
                return win32_inputs[key];
            }

        } run_input_{};

        struct run_wgc_info {
            std::mutex wgc_mutex;
            std::map<std::string,int> info;

            auto push(const std::string& key){
                std::lock_guard lock(this->wgc_mutex);
                info[key]=2;
            }

            auto peek(const std::string& key){
                std::lock_guard lock(this->wgc_mutex);
                if (!info.contains(key)){
                    return 1;
                }
                return info[key];
            }
        } wgc_info {};


        struct run_ocr_resources{

            std::mutex ocr_mutex;
            std::map<std::string,std::unique_ptr<rt::FaaServiceOcrResources>>  ocr_resources;
            nlohmann::json config;

            auto set_config(const nlohmann::json& config){
                this->config=config;
            }

            auto push_ocr(const std::string& key){
                    if (!ocr_resources.contains(key)){
                        auto ocrRes=std::make_unique<rt::FaaServiceOcrResources>();
                        rt::init::faa_ocr_init(*ocrRes,config);
                        ocr_resources.emplace(key,std::move(ocrRes));
                    }

            }

            auto& peek_ocr(const std::string& key){
                std::lock_guard lock(ocr_mutex);
                if (!ocr_resources.contains(key)){
                    push_ocr(key);
                    return ocr_resources[key];
                }
                return ocr_resources[key];
            }

        } ocr_resources{};

        protected:
            nlohmann::json    resources_config_json;
        public:

          FaaServiceOrchestrator(const info::log::Ilog log):ilog(log){}
          ~FaaServiceOrchestrator()=default;

          FaaServiceOrchestrator& initResourceConfig(const std::string& config_path);
          FaaServiceOrchestrator& initNetworkToUdp(std::unique_ptr<net_work::FaaServiceUDPNetwork> network);
          FaaServiceOrchestrator& initThreadPools(int32_t max_threads=0);

          auto& getNetwork() const{
            return *network;
          }

          auto& getRunThreads(){
              return run_threads_;
          }

          auto& getRunThreadPools(){
              return run_thread_pools_;
          }

          auto& running(){
              return is_running;
          }

          auto& getFrameRes(){
              return frame_resources;
          }

          auto& getInputRes(){
              return run_input_;
          }

          auto& getWgcInfo(){
              return wgc_info;
          }

          auto& getLog(){
              return ilog;
          }

          auto& getOcrRes(){
              return ocr_resources;
          }



          FaaServiceOrchestrator& start();
          FaaServiceOrchestrator& join();
    };
}
#endif //FAA_FASTSCREEN_SERVICE_ORCHESTRATOR_H