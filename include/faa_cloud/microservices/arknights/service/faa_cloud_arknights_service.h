//
// Created by HP on 2026/8/26.
//

#ifndef FASTSCREEN_FAA_CLOUD_ARKNIGHTS_SERVICE_H
#define FASTSCREEN_FAA_CLOUD_ARKNIGHTS_SERVICE_H
#include <nlohmann/json.hpp>

#include "faa_cloud_arknights_async.h"
#include "faa-core/fdnn/fcv/faa_dnn_cv_trt.h"
#include "faa-core/fdnn/fcv/processer/ocr/faa_ocr_det_proc.h"
#include "faa-core/fdnn/fcv/processer/ocr/faa_ocr_rec_proc.h"
#include "faa-core/thread_pool/faa_thread_pool.h"
#include "faa-core/wgc_cap/faa_wgc_cap.h"
#include "faa_cloud/common/async/faa_cloud_async.h"
#include "faa_cloud/common/frame/faa_cloud_frame.h"
#include "faa_cloud/common/input/faa_cloud_win32_input.h"
#include "faa_cloud/common/ocr/faa_cloud_ocr.h"
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


        using TensorRTBackend       =fdnn::fcv::FaaCvDnnForTensorRT;
        using TensorRTBackendCtx    =fdnn::fcv::FaaCvCtx;
        using TensorRTOcrDetProc    =fdnn::fcv::processer::ocr::FaaOcrDetProc;
        using TensorRTOcrRecProc    =fdnn::fcv::processer::ocr::FaaOcrRecProc;

        std::unique_ptr<
            common::ocr::FaaCloudOcr<
                TensorRTBackend,
                TensorRTBackendCtx,
                TensorRTOcrDetProc
            >
        > det_ocr;

        std::unique_ptr<
            common::ocr::FaaCloudOcr<
               TensorRTBackend,
               TensorRTBackendCtx,
               TensorRTOcrRecProc
           >
        > rec_ocr;


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

               this->det_ocr        =std::make_unique<
                   common::ocr::FaaCloudOcr<
                       fdnn::fcv::FaaCvDnnForTensorRT,
                       fdnn::fcv::FaaCvCtx,
                       fdnn::fcv::processer::ocr::FaaOcrDetProc
                    >
               >();

               this->rec_ocr        =std::make_unique<
                   common::ocr::FaaCloudOcr<
                       fdnn::fcv::FaaCvDnnForTensorRT,
                       fdnn::fcv::FaaCvCtx,
                       fdnn::fcv::processer::ocr::FaaOcrRecProc
                    >
               >();
           }

           FaaArknightsService(const FaaArknightsService&) = delete;
           FaaArknightsService& operator=(const FaaArknightsService&) = delete;

           FaaArknightsService& setConfig(const nlohmann::json& config);
           FaaArknightsService& setNetwork(std::unique_ptr<net_work::FaaCloudNetwork> network);
           FaaArknightsService& setOperator(info::log::Ilog& log);
           FaaArknightsService& setAsyncOperator(info::log::Ilog& log);

           FaaArknightsService& initOcr       (info::log::Ilog& log);
           FaaArknightsService& initThreadPool(std::unique_ptr<core::thread_pool::FAAThreadPool> thread_pool);
           FaaArknightsService& start(info::log::Ilog& log);


           void addWgcOperator  (info::log::Ilog& log);
           void addSleepOperator(info::log::Ilog& log);
           void addWin32InputOperator(info::log::Ilog& log);

           void addGetAsyncTaskOperator(info::log::Ilog& log);
           void addOcrAsyncOperator    (info::log::Ilog& log);



           void ocrOperator        (info::log::Ilog& log,nlohmann::json& requires_info, common::ocr::FaaCloudOcrAck& ack);
           void win32InputOperator (info::log::Ilog& log, const nlohmann::json& requires_info);


           void loop(info::log::Ilog& log);
           bool isInitialized() const { return !id_str.empty(); };


    };
}
#endif //FASTSCREEN_FAA_CLOUD_ARKNIGHTS_SERVICE_H