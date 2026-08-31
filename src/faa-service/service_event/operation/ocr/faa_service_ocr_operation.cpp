//
// Created by HP on 2026/8/10.
//
#include "faa_service/service_event/operation/ocr/faa_service_ocr_operation.h"
#undef min
#undef max


void faa::service::operation::ocr::get_to_ocr_det(
    nlohmann::json& params,resources::FaaServiceResources& resources,const std::string& global_name,OcrAckResult& result
){
    auto frame=cv::Mat();
    if (!resources.frame_resources.peek(global_name,frame)){
        return;
    }
    if (frame.empty()){
        return;
    }
    auto& [det,rec]=resources.ocr_dnn_resource.peek(global_name);

    cv::Mat blob;
    auto code= fdnn::rt::code::FaaDnnRtCode();
    auto output= fdnn::fcv::FaaCvOutputData();

    det->executor->preProcess(
        *det->proc,frame,blob, fdnn::fcv::processer::ocr::FaaDetPreParam{
         .width = det->width,
         .height = det->height,
         .scalefactor = 1/255.0,
    },code
    ).
    resetInput(det->entrance_layer,{1,3,640,640},*det->ctx).
    postImage(
        det->entrance_layer,blob,*det->ctx, resources::dnn::types_mapper[det->entrance_type]
    ).
    resetOutput(det->exit_layer,*det->ctx,resources::dnn::types_mapper[det->exit_type]);

    auto strat_time=std::chrono::high_resolution_clock::now();
    det->executor->forward(*det->ctx);
    auto end_time=std::chrono::high_resolution_clock::now();
    auto forward_time=std::chrono::duration_cast<std::chrono::milliseconds>(end_time-strat_time).count();
    std::cout<<"Forward time: "<<forward_time<<"ms"<<std::endl;

    det->executor->getOutput(
        det->exit_layer,output,*det->ctx,
         resources::dnn::types_mapper[det->exit_type]
    ).postProcess(
        *det->proc,
        static_cast<float*>(output.toHost()),frame, fdnn::fcv::processer::ocr::FaaDetPostParam{
         det->width,det->height,
         static_cast<float>(params.value("threshold",0.01)),
            static_cast<float>(params.value("unclip_ratio",2.00)),
            params.value("pad",1)
    },code,result.result);


    for (auto rect:result.result.rects){
        cv::Mat cropped = frame(rect).clone();

        // // 1. 转 RGB（模型期望 RGB）
        // if (cropped.channels() == 3) {
        //     cv::cvtColor(cropped, cropped, cv::COLOR_BGR2RGB);
        // } else {
        //     cv::cvtColor(cropped, cropped, cv::COLOR_GRAY2RGB);
        // }
        //
        // // 2. CLAHE 增强（可选）
        // cv::Mat hsv;
        // cv::cvtColor(cropped, hsv, cv::COLOR_RGB2HSV);
        // std::vector<cv::Mat> channels;
        // cv::split(hsv, channels);
        // cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
        // clahe->apply(channels[2], channels[2]);
        // cv::merge(channels, hsv);
        // cv::cvtColor(hsv, cropped, cv::COLOR_HSV2RGB);
        //
        // // 3. 靠左缩放至 320x48（保持宽高比）
        // const int MODEL_H = 48, MODEL_W = 320;
        // double scale = std::min((double)MODEL_H / cropped.rows, (double)MODEL_W / cropped.cols);
        // int new_w = (int)(cropped.cols * scale);
        // int new_h = (int)(cropped.rows * scale);
        // cv::Mat resized;
        // cv::resize(cropped, resized, cv::Size(new_w, new_h), 0, 0, cv::INTER_LINEAR);
        // cv::Mat canvas = cv::Mat::zeros(MODEL_H, MODEL_W, cropped.type());
        // // 靠左+垂直居中
        // resized.copyTo(canvas(cv::Rect(0, (MODEL_H - new_h) / 2, new_w, new_h)));
        // std::cout<<"org width:"<<cropped.cols<<", height:"<<cropped.rows<<std::endl;
        if (cropped.cols <=160){
            cv::resize(cropped,cropped,cv::Size(160,rec->height));
        }

        cv::Mat _blob;
        auto res_result= fdnn::fcv::processer::ocr::FaaResResult();

        rec->executor->
            preProcess(
                  *rec->proc,cropped,_blob, fdnn::fcv::processer::ocr::FaaResParam{
                       .width = cropped.cols,
                       .height = rec->height,
                       .scalefactor = 1 / 255.0
                  },
                  code
             ).
            resetInput(rec->entrance_layer,{
                _blob.size[0],
                _blob.size[1],
                _blob.size[2],
                _blob.size[3]},*rec->ctx).
            postImage(rec->entrance_layer,
                       _blob,*rec->ctx,
                         resources::dnn::types_mapper[rec->entrance_type]
             ).
            resetOutput(rec->exit_layer,*rec->ctx,resources::dnn::types_mapper[rec->exit_type]);

        auto strat_time_=std::chrono::high_resolution_clock::now();
        rec->executor->forward(*rec->ctx);
        auto end_time_=std::chrono::high_resolution_clock::now();
        auto forward_time_=std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ -strat_time_).count();
        std::cout<<"Forward time: "<<forward_time_<<"ms"<<std::endl;

        rec->executor->getOutput(
                  rec->exit_layer,
                  output,*rec->ctx,
                  resources::dnn::types_mapper[rec->exit_type]
             );

        auto shape=rec->executor->getOutputShapes(rec->exit_layer,*rec->ctx);
         if (shape.size()!=3){
            continue;
         }
        rec->executor->postProcess(*rec->proc,
                  static_cast<float*>(output.toHost()),
                  fdnn::fcv::processer::ocr::FaaPostParam{
                  .model_vocal_size = static_cast<int>(shape[2]),
                  .seq_len         =  static_cast<int>(shape[1]),
             },res_result,code
        );

        std::string text;

        for (const auto& id : res_result.ids){
            if (id-1<rec->proc->get_vocal().size()){
                text+=rec->proc->get_vocal()[id-1];
            }
        }
        result.key_values.emplace_back(text,rect);
    }
    delete[] static_cast<float*>(output.toHost());

}
