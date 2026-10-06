//
// Created by HP on 2026/9/5.
//

#ifndef FASTSCREEN_FAA_CLOUD_OCR_OPER_H
#define FASTSCREEN_FAA_CLOUD_OCR_OPER_H
#include <opencv2/core/mat.hpp>

#include "faa_cloud_ocr.h"
#include "faa-core/fdnn/fcv/faa_dnn_cv_trt.h"
#include "faa-core/fdnn/fcv/processer/ocr/faa_ocr_det_proc.h"
#include "faa-core/fdnn/fcv/processer/ocr/faa_ocr_rec_proc.h"
#include "faa-core/fdnn/runtime/faa_dnn_rt_code.h"

namespace faa::cloud::common::ocr{

    template<typename B,typename C,typename P>
    void ocrDetOper(nlohmann::json& config,
                    FaaCloudOcr<B,C,P>& det_ocr,cv::Mat& img,
                    fdnn::fcv::FaaCvOutputData& output,
                    FaaCloudOcrAck& ack
                    ){

        auto code= fdnn::rt::code::FaaDnnRtCode();
        cv::Mat blob;
        int KeepX  = config.value("KeepX",1);
        int KeepY  = config.value("KeepY",1);
        int KeepW  = config.value("KeepW",img.cols);
        int KeepH  = config.value("KeepH",img.rows);

        cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1);
        cv::rectangle(mask, cv::Rect(KeepX, KeepY, KeepW, KeepH), cv::Scalar(255), cv::FILLED);
        img.setTo(cv::Scalar(0, 0, 0), ~mask);   // mask 为 0 的位置置黑

        int width  = config.value("width" ,det_ocr.config.value("width",640));
        int height = config.value("height",det_ocr.config.value("height",640));

        det_ocr.executor->
        preProcess(*det_ocr.process,img,blob, fdnn::fcv::processer::ocr::FaaDetPreParam{
             .width  = width,
             .height = height,
             .scalefactor = 1/255.0,
              },code
            ).
            resetInput(
                    det_ocr.entrance_layer,
                    std::vector<int64_t>{
                            blob.size[0],
                            blob.size[1],
                            blob.size[2],
                            blob.size[3]
                    },
                    det_ocr.ctx
            ).
            postImage(
                det_ocr.entrance_layer,blob,
                det_ocr.ctx,
                types_mapping[det_ocr.entrance_type]
            ).
            resetOutput(
                det_ocr.exit_layer,
                det_ocr.ctx,
                types_mapping[det_ocr.exit_type]
            );

        auto start = std::chrono::high_resolution_clock::now();
        det_ocr.executor->
            forward(det_ocr.ctx);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout<<"forward time: "<<std::chrono::duration<double, std::milli>( end - start ).count()<<" ms"<<std::endl;

        start = std::chrono::high_resolution_clock::now();
        det_ocr.executor->getOutput(
                        det_ocr.exit_layer,
                        output,
                        det_ocr.ctx,
                        types_mapping[det_ocr.exit_type]
            );
        end = std::chrono::high_resolution_clock::now();
        std::cout<<"getOutput time: "<<std::chrono::duration<double, std::milli>( end - start ).count()<<" ms"<<std::endl;

        det_ocr.executor->
            postProcess(
                    *det_ocr.process,
                    static_cast<float*>(output.toHost()),img,
                    fdnn::fcv::processer::ocr::FaaDetPostParam{
                        width,height,
                        static_cast<float>(config.value("threshold",0.01)),
                        static_cast<float>(config.value("unclip_ratio",2.00)),
                        config.value("pad",1)
                    },
                    code,
                    ack.det_result
        );

    }

    template<typename B,typename C,typename P>
    void ocrRecOper(nlohmann::json& config,
                    FaaCloudOcr<B,C,P>& rec_ocr,cv::Mat& img,
                    fdnn::fcv::FaaCvOutputData& output,
                    FaaCloudOcrAck& ack){

        const int min_width      = rec_ocr.config.value("min_width",160);
        const int max_width      = rec_ocr.config.value("max_width",3200);
        const int height         = rec_ocr.config.value("height",48);
        auto key_word      = config.value("key_word","");
        auto code= fdnn::rt::code::FaaDnnRtCode();

        for (const auto& box : ack.det_result.rects){

            cv::Mat roi = img(box);
            cv::resize(roi,roi,cv::Size(roi.cols,height));

            if (roi.cols<min_width){
                cv::resize(roi,roi,cv::Size(min_width,height));
            }
            if (roi.cols>max_width){
                cv::resize(roi,roi,cv::Size(max_width,height));
            }


            cv::Mat blob;
            auto res_result= fdnn::fcv::processer::ocr::FaaResResult();
            rec_ocr.executor->
                        preProcess(
                            *rec_ocr.process,roi,blob,
                            fdnn::fcv::processer::ocr::FaaResParam{
                                 .width  = roi.cols,
                                 .height = roi.rows,
                                 .scalefactor = 1/255.0,
                            },
                            code
                        ).
                        resetInput(
                            rec_ocr.entrance_layer,
                            std::vector<int64_t>{
                                    blob.size[0],
                                    blob.size[1],
                                    blob.size[2],
                                    blob.size[3]
                            },
                            rec_ocr.ctx
                        ).
                        postImage(
                            rec_ocr.entrance_layer,blob,
                            rec_ocr.ctx,
                            types_mapping[rec_ocr.entrance_type]
                        ).
                        resetOutput(
                            rec_ocr.exit_layer,
                            rec_ocr.ctx,
                            types_mapping[rec_ocr.exit_type]
                        ).
                        forward(rec_ocr.ctx);

            rec_ocr.executor->getOutput(
                  rec_ocr.exit_layer,
                  output,
                  rec_ocr.ctx,
                  types_mapping[rec_ocr.exit_type]
            );
            auto shape=rec_ocr.executor->getOutputShapes(rec_ocr.exit_layer,rec_ocr.ctx);
            if (shape.size()!=3){
                continue;
            }
            rec_ocr.executor->postProcess(
                *rec_ocr.process,
                      static_cast<float*>(output.toHost()),
                      fdnn::fcv::processer::ocr::FaaPostParam{
                          .model_vocal_size = static_cast<int>(shape[2]),
                          .seq_len         =  static_cast<int>(shape[1]),
                      },
                      res_result,code
            );

            std::string text;

            for (const auto& id : res_result.ids){
                if (id-1<rec_ocr.process->get_vocal().size()){
                    text+=rec_ocr.process->get_vocal()[id-1];
                }
            }
            ack.rec_result.emplace_back(text,box);
            if (!key_word.empty() && key_word==text){
                break;
            }
        }
    }



}
#endif //FASTSCREEN_FAA_CLOUD_OCR_OPER_H