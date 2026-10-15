//
// Created by HP on 2026/10/9.
//

#ifndef FASTSCREEN_FAA_CLOUD_GENERAL_H
#define FASTSCREEN_FAA_CLOUD_GENERAL_H

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   // 阻止 windows.h 包含旧版 winsock.h
#endif

#include "faa_cloud/common/input/faa_cloud_win32_input.h"
#include "faa_cloud/common/ocr/faa_cloud_ocr.h"
#include "faa_cloud/common/yolo/faa_cloud_yolo.h"
#include "faa_core/faa_export.h"
#include "faa_cloud/microservices/base/service/faa_cloud_base_services.h"
#include "faa_core/fdnn/fcv/faa_dnn_cv_trt.h"
#include "faa_core/fdnn/fcv/processer/ob/faa_yolo_proc.h"
#include "faa_core/fdnn/fcv/processer/ocr/faa_ocr_det_proc.h"
#include "faa_core/fdnn/fcv/processer/ocr/faa_ocr_rec_proc.h"

namespace faa::cloud::microservices::general {

    class FAA_CPP_EXPORT FaaCloudGeneralServices : public base::FaaCloudBaseServices<FaaCloudGeneralServices> {

    protected:



        using TensorRTBackend       =fdnn::fcv::FaaCvDnnForTensorRT;
        using TensorRTBackendCtx    =fdnn::fcv::FaaCvCtx;
        using OcrDetProc    =fdnn::fcv::processer::ocr::FaaOcrDetProc;
        using OcrRecProc    =fdnn::fcv::processer::ocr::FaaOcrRecProc;
        using YoloProc      =fdnn::fcv::processer::ob::FaaYoloProc;

        using TensorOcrDet = std::unique_ptr<
            common::ocr::FaaCloudOcr<
                TensorRTBackend,
                TensorRTBackendCtx,
                OcrDetProc
            >
        > ;
        using TensorOcrRec = std::unique_ptr<
            common::ocr::FaaCloudOcr<
                TensorRTBackend,
                TensorRTBackendCtx,
                OcrRecProc
            >
        > ;
        std::pair<TensorOcrDet,TensorOcrRec> tensor_ocr_pair;

        common::input::win32::FaaCloudWin32Input win32_input;

        std::map<
            std::string,
            std::unique_ptr<
                common::yolo::FaaCloudYolo<TensorRTBackend,TensorRTBackendCtx,YoloProc>
            >
        > yolo_map;


        const char* isCapturing="isCapturing";

    public:

        void initTensorOcr  (info::log::Ilog* log = nullptr);
        void initYolo       (info::log::Ilog* log = nullptr);

        void addCommand_loop(info::log::Ilog* log = nullptr);
        void addCommand     (info::log::Ilog* log = nullptr);


        void addExitCmd    (info::log::Ilog* log = nullptr);
        void addIsCaptureCmd (info::log::Ilog* log = nullptr);


        void addGetFramesCmd(info::log::Ilog* log = nullptr);
        void addCapThread(info::log::Ilog* log = nullptr);
        void addNetworkThread(info::log::Ilog* log = nullptr);


        void addRequestOper   (info::log::Ilog* log = nullptr);
        void addWgcCaptureOper(info::log::Ilog* log = nullptr);

        void addOcrOper     (info::log::Ilog* log = nullptr);
        void addYoloOper    (info::log::Ilog* log = nullptr);
        void addGetYoloModel(info::log::Ilog* log = nullptr);

        void addWin32InputOper(info::log::Ilog* log = nullptr);

        // void test(){};


    };
}


#endif //FASTSCREEN_FAA_CLOUD_GENERAL_H