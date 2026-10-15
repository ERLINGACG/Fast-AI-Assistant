//
// Created by HP on 2026/10/13.
//
#include "faa_cloud/common/ocr/faa_cloud_ocr_oper.h"
#include "faa_cloud/microservices/general/service/faa_cloud_general.h"
using faa::cloud::microservices::general::FaaCloudGeneralServices;

void FaaCloudGeneralServices::addOcrOper(info::log::Ilog* log) {
    this->operat.push("ocr",[this,log]

    (net_work::FaaCloudNetworkLoad& load,sockaddr_in response_addr){
        // log->info("ocr to {}",load.unserialize().dump());
        cv::Mat frame;
        this->cap.frame_resources.peek(frame);
        if (frame.empty()) {
            log->error("frame is empty");
            load.setBody({
                {"error","frame is empty"}
            });
            return 0;
        }

        fdnn::fcv::FaaCvOutputData output{};
        common::ocr::FaaCloudOcrAck ack;

        faa::cloud::common::ocr::ocrDetOper(
            load.body().data,*this->tensor_ocr_pair.first,
            frame,output,ack
        );

        faa::cloud::common::ocr::ocrRecOper(
            load.body().data,*this->tensor_ocr_pair.second,
            frame,output,ack
        );

        delete[] static_cast<float*>(output.toHost());;
        ack.detail();

        load.setBody(ack.data);
        this->network.get().sendLoad(
            load,
            response_addr
        );
        return 0;
    });

}


