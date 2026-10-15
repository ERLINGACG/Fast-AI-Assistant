//
// Created by HP on 2026/10/4.
//

#ifndef FASTSCREEN_FAA_CLOUD_YOLO_OPER_H
#define FASTSCREEN_FAA_CLOUD_YOLO_OPER_H
#include <mutex>
#include <opencv2/core/mat.hpp>

#include "faa_cloud_yolo.h"
#include "faa_core/fdnn/data/faa_dnn_data_type.h"
#include "faa_core/fdnn/fcv/processer/ob/faa_yolo_proc.h"

namespace  faa::cloud::common::yolo {

    template<typename B,typename C,typename P>
    void yoloOper(nlohmann::json& config,
                      FaaCloudYolo<B,C,P>& yolo,cv::Mat& img,  fdnn::fcv::FaaCvOutputData& output,
                      fdnn::fcv::processer::ob::FaaYoloResult& result,info::log::Ilog* ilog = nullptr)
    {
        if (ilog) ilog->debug("yolo oper");
         std::lock_guard<std::mutex> lock(yolo.lock);
         if (img.empty()) {
            return;
         }
         cv::Mat blob;
         auto params = fdnn::fcv::processer::ob::FaaYoloParams{
            .input_width  = config.value("input_width",640),
            .input_height = config.value("input_height",640),
             .isDraw   = config.value("isDraw",true),
             .conf     = static_cast<float>(config.value("conf",0.5)),
             .iou      = static_cast<float>(config.value("iou",0.4)) ,
             .maxScore = static_cast<float>(config.value("maxScore",0.1)),
         };


         yolo.executor->
            preProcess(*(yolo.process),img,blob, params).
            resetInput(yolo.entrance_layer,
                std::vector<int64_t>{
                        blob.size[0],
                        blob.size[1],
                        blob.size[2],
                        blob.size[3]
                },
                yolo.ctx
                ).
                postImage(
                       yolo.entrance_layer,blob,
                       yolo.ctx,
                       fdnn::data::type::fdnn_types_mapping[yolo.entrance_type]
                ).resetOutput(
                       yolo.exit_layer,
                       yolo.ctx,
                       fdnn::data::type::fdnn_types_mapping[yolo.exit_type]
                );

                yolo.executor->
                    forward(yolo.ctx);



                std::cout<<"success"<<std::endl;

                yolo.executor->getOutput(
                                    yolo.exit_layer,
                                    output,
                                    yolo.ctx,
                                    fdnn::data::type::fdnn_types_mapping[yolo.exit_type]
                                );

                auto* output_data=static_cast<float*>(output.toHost());

                yolo.executor->postProcess(
                    *yolo.process,
                    output_data,
                    img,params,result
                );


    }
}
#endif //FASTSCREEN_FAA_CLOUD_YOLO_OPER_H