//
// Created by HP on 2026/8/26.
//

#ifndef FASTSCREEN_FAA_CLOUD_ARKNIGHTS_SERVICE_H
#define FASTSCREEN_FAA_CLOUD_ARKNIGHTS_SERVICE_H
#include <nlohmann/json.hpp>

#include "faa-core/thread_pool/faa_thread_pool.h"
#include "faa-core/wgc_cap/faa_wgc_cap.h"
#include "faa_cloud/common/frame/faa_cloud_frame.h"
#include "faa_cloud/network/faa_cloud_network.h"

namespace faa::cloud::microservices::arknigths{


    class FaaArknightsService{

        nlohmann::json config_;
        std::atomic<bool> running=true;
        std::atomic<bool> get_frames_running=false;

        std::unique_ptr<net_work::FaaCloudNetwork>             network_;
        std::unique_ptr<core::thread_pool::FAAThreadPool>      thread_pool_;

        std::unique_ptr<frame::Frames> frame_resources;
        std::thread get_frames_thread;

        std::string id_str={};

        std::map<std::string,std::function<void()>> operator_map={};

        public:
           FaaArknightsService()=default;
           FaaArknightsService& setConfig(const nlohmann::json& config);
           FaaArknightsService& setNetwork(std::unique_ptr<net_work::FaaCloudNetwork> network);
           FaaArknightsService& setOperator(info::log::Ilog& log);
           FaaArknightsService& initThreadPool(std::unique_ptr<core::thread_pool::FAAThreadPool> thread_pool);
           FaaArknightsService& start(info::log::Ilog& log);



           void loop(info::log::Ilog& log);

           bool isInitialized() const { return !id_str.empty(); };


    };
}
#endif //FASTSCREEN_FAA_CLOUD_ARKNIGHTS_SERVICE_H