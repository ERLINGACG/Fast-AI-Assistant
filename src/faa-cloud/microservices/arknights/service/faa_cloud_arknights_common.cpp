//
// Created by HP on 2026/9/7.
//
#include "faa-core/win_utils/faa_win_utils.h"
#include "faa_cloud/common/ocr/faa_cloud_ocr_oper.h"
#include "faa_cloud/microservices/arknights/service/faa_cloud_arknights_service.h"
using faa::cloud::microservices::arknigths::FaaArknightsService;


void FaaArknightsService::ocrOperator(info::log::Ilog& log,nlohmann::json& requires_info, common::ocr::FaaCloudOcrAck& ack){
    cv::Mat frame;
    this->frame_resources->peek(frame);

    if (!frame.empty()){

        fdnn::fcv::FaaCvOutputData output{};
        faa::cloud::common::ocr::ocrDetOper(
            requires_info,*this->det_ocr,
            frame,output,ack
        );

        faa::cloud::common::ocr::ocrRecOper(
            requires_info,*this->rec_ocr,
            frame,output,ack
        );
        delete[] static_cast<float*>(output.toHost());;
        ack.detail();
    }
}

void FaaArknightsService::ocrOperator(
    info::log::Ilog& log,
    net_work::FaaCloudNetworkLoad& load,
    common::ocr::FaaCloudOcrAck& ack

    ) {

    cv::Mat frame;
    this->frame_resources->peek(frame);
    log.debug("params:threshold:{}",load.body().data.value("threshold",0.0));
    if (!frame.empty()){
        fdnn::fcv::FaaCvOutputData output{};
        faa::cloud::common::ocr::ocrDetOper(
            load.body().data,*this->det_ocr,
            frame,output,ack
        );

        faa::cloud::common::ocr::ocrRecOper(
            load.body().data,*this->rec_ocr,
            frame,output,ack
        );
        delete[] static_cast<float*>(output.toHost());;
        ack.detail();

    }
}

void FaaArknightsService::win32InputOperator(info::log::Ilog& log, const nlohmann::json& requires_info)
{
    if (!this->win32input->isInit()){
        auto hwnd=core::win_utils::GetWindowHandle(this->config_.value("target","") );
        if (hwnd==nullptr){
            hwnd=core::win_utils::GetWindowHandleByClass(this->config_.value("target",""));
        }
        if (hwnd==nullptr){
            hwnd=core::win_utils::GetWindowHandleByName(this->config_.value("target",""));
        }
        if (hwnd==nullptr){
            log.error("get window handle failed");
            return;
        }
        log.debug("hwnd is found");
        this->win32input->set(
            std::make_unique<input::windows::WindowsInput>(hwnd)
        );
    }

    for(auto& it:requires_info.value("inputs",nlohmann::json::array())){
        auto type_name= it.value("type","");
        if (this->win32input->operators.contains(type_name) ){
            this->win32input->operators[type_name](it);
        }

    }
}
