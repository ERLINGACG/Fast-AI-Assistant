//
// Created by HP on 2026/8/26.
//

#ifndef FASTSCREEN_FAA_CLOUD_ARKNIGHTS_SERVICE_H
#define FASTSCREEN_FAA_CLOUD_ARKNIGHTS_SERVICE_H
#include <nlohmann/json.hpp>

#include "faa_cloud_arknights_async.h"
#include "faa-core/thread_pool/faa_thread_pool.h"
#include "faa-core/wgc_cap/faa_wgc_cap.h"
#include "faa_cloud/common/async/faa_cloud_async.h"
#include "faa_cloud/common/frame/faa_cloud_frame.h"
#include "faa_cloud/common/input/faa_cloud_win32_input.h"
#include "faa_cloud/network/faa_cloud_network.h"

namespace faa::cloud::microservices::arknigths{


    class FaaArknightsService{

        nlohmann::json config_;
        std::atomic<bool> running=true;
        std::atomic<bool> get_frames_running=false;

        std::unique_ptr<net_work::FaaCloudNetwork>             network_;
        std::unique_ptr<core::thread_pool::FAAThreadPool>      thread_pool_;


        std::thread get_frames_thread;

        std::string id_str={};

        std::unique_ptr<frame::Frames> frame_resources;
        std::unique_ptr<common::input::win32::FaaCloudWin32Input> win32input;
        common::async::AsyncTasks async_tasks;

        std::map<
            std::string,
            std::function<
                void(nlohmann::json& requires_info,sockaddr_in& response_addr)
            >
        > operator_map={};

        public:
           FaaArknightsService(){
               this->frame_resources=std::make_unique<frame::Frames>();
               this->win32input     =std::make_unique<common::input::win32::FaaCloudWin32Input>();
           }
           FaaArknightsService& setConfig(const nlohmann::json& config);
           FaaArknightsService& setNetwork(std::unique_ptr<net_work::FaaCloudNetwork> network);
           FaaArknightsService& setOperator(info::log::Ilog& log);
           FaaArknightsService& setAsyncOperator(info::log::Ilog& log);
           FaaArknightsService& initThreadPool(std::unique_ptr<core::thread_pool::FAAThreadPool> thread_pool);
           FaaArknightsService& start(info::log::Ilog& log);

           void loop(info::log::Ilog& log);

           bool isInitialized() const { return !id_str.empty(); };


    };
}
#endif //FASTSCREEN_FAA_CLOUD_ARKNIGHTS_SERVICE_H