//
// Created by HP on 2026/7/17.
//
#include "../../../include/faa_service/runtime/faa_service_runtime.h"

bool faa::service::rt::init_resources(FaaServiceRuntimeResources& resources, const FaaResourceConfig& config,Ilog& log){

     resources.wgc_cap_resources.
          window_capture=std::make_unique<FAAWindowCapture>(FAAWindowCapture(config.target_windows,log)
               .initD3d11(resources.wgc_cap_resources.d3d_ctx)
               .initWgcRtCtx(resources.wgc_cap_resources.wgc_rt).
                startSession());

     if (!resources.wgc_cap_resources.window_capture->is_init()){
          log.error("init window capture failed");
          resources.is_init=false;
          return false;
     }


     auto init_ocr_dnn=[&](){

          auto init_det=[&](const std::string& entrance_layer,
                            const std::string& entrance_type,
                            const std::string& exit_layer,
                            const std::string& exit_type,
                            const std::string& path,
                            const int32_t width, const int32_t height,
                            nlohmann::json& layers
          ){
               resources.ocr_resources.det_dnn.proc=std::make_unique<FaaOcrDetProc>();
               resources.ocr_resources.det_dnn.init(
                  entrance_layer,
                  entrance_type,
                  exit_layer,
                  exit_type,
                  width,
                  height,
                  std::make_unique<FaaCvDnnForTensorRT>(path)
               );

               for (const auto& info:layers.items()){

                    if (info.value().contains("input")){

                         log.debug("layers:{}",info.value().dump());
                         auto input=info.value()["input"];
                         resources.ocr_resources.det_dnn.input(
                              input["name"].get<std::string>(),
                              input["dims"].get<std::vector<int64_t>>()
                         );
                    }

                    if (info.value().contains("output")){

                         log.debug("layers:{}",info.value().dump());
                         auto output=info.value()["output"];
                         resources.ocr_resources.det_dnn.output(
                              output["name"].get<std::string>(),
                               output["type"].get<std::string>()
                         );
                    }
               }

          };
          auto init_rec=[&](const std::string& vocab_path,
                            const std::string& entrance_layer,
                            const std::string& entrance_type,
                            const std::string& exit_layer,
                            const std::string& exit_type,
                            const std::string& path,
                            const int32_t width, const int32_t height,
                            nlohmann::json& layers
          ){
               resources.ocr_resources.rec_dnn.proc=std::make_unique<FaaOcrRecProc>(vocab_path);
               resources.ocr_resources.rec_dnn.init(
                  entrance_layer,
                  entrance_type,
                  exit_layer,
                  exit_type,
                  width,
                  height,
                  std::make_unique<FaaCvDnnForTensorRT>(path)
               );
               for (const auto& info:layers.items()){

                    if (info.value().contains("input")){

                         log.debug("layers:{}",info.value().dump());
                         auto input=info.value()["input"];
                         resources.ocr_resources.rec_dnn.input(
                              input["name"].get<std::string>(),
                              input["dims"].get<std::vector<int64_t>>()
                         );
                    }

                    if (info.value().contains("output")){

                         log.debug("layers:{}",info.value().dump());
                         auto output=info.value()["output"];
                         resources.ocr_resources.rec_dnn.output(
                              output["name"].get<std::string>(),
                               output["type"].get<std::string>()
                         );
                    }
               }
          };
          for (const auto& it : config.model_config.items()){

               if (it.key()=="det"){
                    auto v=it.value();
                    init_det(
                         v["entrance"]["name"].get<std::string>(),
                         v["entrance"]["type"].get<std::string>(),
                         v["exit"]["name"].get<std::string>(),
                         v["exit"]["type"].get<std::string>(),
                         v["path"].get<std::string>(),
                         v["width"].get<int32_t>(),
                         v["height"].get<int32_t>(),
                            v["layers"]
                    );
               }
               if (it.key()=="rec"){
                    auto v=it.value();
                    init_rec(
                         v["vocab_path"].get<std::string>(),
                         v["entrance"]["name"].get<std::string>(),
                         v["entrance"]["type"].get<std::string>(),
                         v["exit"]["name"].get<std::string>(),
                         v["exit"]["type"].get<std::string>(),
                         v["path"].get<std::string>(),
                         v["width"].get<int32_t>(),
                         v["height"].get<int32_t>(),
                         v["layers"]
                    );
               }
          }
          return true;
     };

     try{
          resources.is_init=init_ocr_dnn();
     }catch (const std::exception& e){
          log.error("init ocr dnn failed: {}",e.what());
          resources.is_init=false;
          return false;
     }


     return  true;

}

void faa::service::rt::get_frame(FaaServiceRuntimeResources& resources, FaaFrame& frame){
     if (!resources.is_init){
          return;
     }
     while (frame.frame.empty()){
          resources.wgc_cap_resources.
               window_capture->getFrame(frame.wgc_texture_data,frame.frame);
     }
}

void faa::service::rt::ocr_proc_frame(FaaServiceRuntimeResources& resources, FaaFrame& frame,FaaSerOcrResult& result){
     if (!resources.is_init){
          return;
     }
     cv::Mat blob;
     auto code=fdnn::rt::code::FaaDnnRtCode::Success;
     auto& [width, height, entrance_type, entrance_layer,exit_layer,exit_type, executor, proc, ctx]=resources.ocr_resources.det_dnn;

     auto output= fdnn::fcv::FaaCvOutputData();
     executor->preProcess(*proc,frame.frame,blob, fdnn::fcv::processer::ocr::FaaDetPreParam{
         .width = width,
         .height = height,
         .scalefactor = 1.0/255.0,
     },code).
     postImage(
             entrance_layer,
             blob,
             *ctx,
           mapper[entrance_type]).
     forward(*ctx).
     getOutput(exit_layer,output,*ctx,mapper[exit_type]).
     postProcess(*proc
          ,static_cast<float*>(output.toHost()),
          frame.frame,
          fdnn::fcv::processer::ocr::FaaDetPostParam{
               width,height,
               0.01,
               2.0,
               5,
     },code,result.result);

     auto& rec_executor=resources.ocr_resources.rec_dnn;

     for (auto rect:result.result.rects){
          cv::Mat cropped = frame.frame(rect).clone();
          cv::Mat _blob;
          auto res_result= fdnn::fcv::processer::ocr::FaaResResult();
          // auto output2= fdnn::fcv::FaaCvOutputData();

          rec_executor.executor->preProcess(
               *rec_executor.proc,cropped,_blob, fdnn::fcv::processer::ocr::FaaResParam{
                    .width = rec_executor.width,
                    .height = rec_executor.height,
                    .scalefactor = 1 / 255.0
               },
               code
          ).postImage(rec_executor.entrance_layer,
                    _blob,*rec_executor.ctx,
                      mapper[rec_executor.entrance_type]
                      )
          .forward(*rec_executor.ctx)
          .getOutput(
               rec_executor.exit_layer,
               output,*rec_executor.ctx,
               mapper[rec_executor.exit_type]
          )
          .postProcess(*rec_executor.proc,
               static_cast<float*>(output.toHost()),
               fdnn::fcv::processer::ocr::FaaPostParam{
               .model_vocal_size = 18385,
               .seq_len = 40,
          },res_result,code);


          std::string text;
          std::string ids="[";
          for (const auto& id : res_result.ids){
               if (id-1<rec_executor.proc->get_vocal().size()){
                    text+=rec_executor.proc->get_vocal()[id-1];
               }
               ids+=std::to_string(id)+",";
          }
          ids.pop_back();
          ids+="]";
          std::cout<<"ids: "<<ids<<" | "<<"text: "<<text<<std::endl;
          result.keys.emplace_back(text,rect);
     }
    delete[] static_cast<float*>(output.toHost());

}

