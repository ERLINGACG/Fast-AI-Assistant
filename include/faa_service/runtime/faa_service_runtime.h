//
// Created by HP on 2026/7/17.
//

#ifndef FASTSCREEN_FAA_SERVICE_RUNTIME_H
#define FASTSCREEN_FAA_SERVICE_RUNTIME_H
#undef ERROR
#include "../faa_result.h"
#include "faa-core/fdnn/fcv/faa_dnn_cv_trt.h"
#include "faa-core/fdnn/fcv/processer/ocr/faa_ocr_det_proc.h"
#include "faa-core/fdnn/fcv/processer/ocr/faa_ocr_rec_proc.h"
#include "faa-core/fdnn/runtime/faa_dnn_executor.h"
#include "faa-core/wgc_cap/faa_wgc_cap.h"

namespace faa::service::rt{
     using core::cap::wgc::FAAWindowCapture;
     using core::cap::wgc::WGCRunTimeContext;
     using core::cap::wgc::D3DDeviceContext;
     using core::cap::wgc::WgcTextureData;
     using fdnn::rt::FaaDnnExecutor;
     using fdnn::fcv::FaaCvDnnForTensorRT;
     using fdnn::fcv::FaaCvCtx;
     using fdnn::fcv::processer::ocr::FaaOcrDetProc;
     using fdnn::fcv::processer::ocr::FaaOcrRecProc;
     using info::log::Ilog;


     inline  std::map<std::string,gdlz::trt::core::cuda::DataType> mapper{
          {"fp32",gdlz::trt::core::cuda::DataType::FLOAT32},
          {"fp16",gdlz::trt::core::cuda::DataType::FLOAT16},
          {"int8",gdlz::trt::core::cuda::DataType::INT8},
     };

     struct  FaaResourceConfig{
         std::string target_windows;
         nlohmann::json model_config;
     };

    template<typename B,typename C,typename P>
    struct FaaCvDnn{

         int32_t width{};
         int32_t height{};
         std::string entrance_type{};
         std::string entrance_layer{};
         std::string exit_layer{};
         std::string exit_type{};
         std::unique_ptr<FaaDnnExecutor<B,C>> executor;
         std::unique_ptr<P> proc;
         std::unique_ptr<C> ctx;

         template<typename... Args>
         auto init(

                   const std::string& entrance_layer,
                   const std::string& entrance_type,
                   const std::string& exit_layer,
                   const std::string& exit_type,
                   int32_t width,int32_t height,
                   Args&&... args
                   ) {

             executor=std::make_unique<FaaDnnExecutor<B,C>>(std::forward<Args>(args)...);

             ctx=std::make_unique<C>();

             this->width=width;
             this->height=height;
             this->entrance_type=entrance_type;
             this->entrance_layer=entrance_layer;
             this->exit_type=exit_type;
             this->exit_layer=exit_layer;

             executor->initEngine();
             executor->initCtx(*this->ctx);
         }
         auto input(const std::string& name, const std::vector<int64_t>& dims){
             executor->setInput(name,dims,*ctx);
         }
         auto output(const std::string& name,const std::string& type) {
             executor->setOutput(name,mapper[type],*ctx);
         }
    };

     struct FaaServiceOcrResources{
         bool is_init=false;
         FaaCvDnn<FaaCvDnnForTensorRT,FaaCvCtx,FaaOcrDetProc> det_dnn;
         FaaCvDnn<FaaCvDnnForTensorRT,FaaCvCtx,FaaOcrRecProc> rec_dnn;
     };

     struct FaaWgcCapResources{
         std::unique_ptr<FAAWindowCapture> window_capture;
         D3DDeviceContext d3d_ctx;
         WGCRunTimeContext wgc_rt;
     };

     struct FaaServiceRuntimeResources{

         FaaWgcCapResources wgc_cap_resources{};
         FaaServiceOcrResources ocr_resources{};

         bool is_init=false;
         ~FaaServiceRuntimeResources()=default;

     };

     struct FaaFrame{
         WgcTextureData wgc_texture_data{};
         cv::Mat frame{};
     };

     bool init_resources(FaaServiceRuntimeResources& resources, const FaaResourceConfig& config,Ilog& log);
     void get_frame     (FaaServiceRuntimeResources& resources, FaaFrame& frame);
     void ocr_proc_frame    (FaaServiceRuntimeResources& resources, FaaFrame& frame,FaaSerOcrResult& result);

}
#endif //FASTSCREEN_FAA_SERVICE_RUNTIME_H