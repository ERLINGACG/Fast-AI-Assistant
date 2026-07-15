//
// Created by HP on 2026/7/8.
//
#include <iostream>
#include <string>
#include <gdlz/trt/cv/ocr/trt_cv_ocr_data.h>
#include <gdlz/trt/cv/core/trt_cv_core.h>
#include <opencv2/opencv.hpp>

#include "faa-core/fdnn/fcv/faa_dnn_cv_trt.h"
#include "faa-core/fdnn/fcv/processer/ocr/faa_ocr_det_proc.h"
#include "faa-core/fdnn/fcv/processer/ocr/faa_ocr_rec_proc.h"
#include "faa-core/fdnn/runtime/faa_dnn_executor.h"
using gdlz::trt::icv::core::TensorRTCvCoreFramework;
using gdlz::trt::icv::core::TensorRTCvCtx;
using gdlz::trt::core::cuda::DataType;
using gdlz::trt::core::cuda::TensorRTCudaData;

class FaaOCRProcessor{
    public:
     int PreProcess(cv::Mat& img,cv::Mat& blob){
         if (img.empty()){
             return -1;
         };
         auto clone_img=img.clone();
         cv::resize(clone_img,clone_img,cv::Size(640,640));
         cv::dnn::blobFromImage(clone_img,blob,1/255.0,cv::Size(640,640),false,false);
         return 0;
     }
     int PostProcess(float* host_float,cv::Mat& img,cv::Mat& blob,std::vector<cv::Rect>& rects){
         if (host_float==nullptr){
             return -1;
         }
         cv::Mat probMap(640, 640, CV_32F);
         cv::Mat(640, 640, CV_32F, host_float).copyTo(probMap);
         double p_min, p_max;
         cv::minMaxLoc(probMap, &p_min, &p_max);
         if (p_max > p_min) {
             probMap = (probMap - p_min) / (p_max - p_min);
         }
         cv::Mat bin;
         cv::threshold(probMap, bin, 0.01, 1.0, cv::THRESH_BINARY);
         bin.convertTo(bin, CV_8U, 255);
         std::vector<std::vector<cv::Point>> contours;
         std::vector<cv::Vec4i> hierarchy;
         cv::findContours(bin, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

         const double min_area = std::max(5.0, 30.0 *
             img.cols * img.rows / (640 * 640)
         );
         float unclip_ratio=2.0f;
         int PAD=5;
         std::vector<cv::RotatedRect> textBoxes;
         for (const auto& c : contours) {
             double area = cv::contourArea(c);
             if (area < min_area) continue;
             cv::RotatedRect r = cv::minAreaRect(c);
             // unclip 膨胀：放大宽高，保持中心和角度不变
             r.size.width  *= unclip_ratio;
             r.size.height *= unclip_ratio;
             textBoxes.push_back(r);
         }
         float sx = static_cast<float>(img.cols) / static_cast<float>(640);
         float sy = static_cast<float>(img.rows) / static_cast<float>(640);
         for (const auto& rect : textBoxes) {
             // 1. 映射到原图坐标
             cv::Point2f center(rect.center.x * sx, rect.center.y * sy);
             cv::Size2f size(rect.size.width * sx, rect.size.height * sy);
             cv::RotatedRect scaled_rect(center, size, rect.angle);

             // 2. 取轴对齐包围盒 + 留白，直接从原图裁剪
             cv::Rect bbox = scaled_rect.boundingRect();
             bbox.x      = std::max(0, bbox.x - PAD);
             bbox.y      = std::max(0, bbox.y - PAD);
             bbox.width  = std::min(img.cols - bbox.x, bbox.width + 2 * PAD);
             bbox.height = std::min(img.rows - bbox.y, bbox.height + 2 * PAD);
             if (bbox.width <= 0 || bbox.height <= 0) continue;
             cv::rectangle(img, bbox, cv::Scalar(0, 0, 255), 2);
             rects.push_back(bbox);
         }
         return 0;

     }
};


auto run_test(const std::string& config,cv::Mat& img){
    cv::Mat blob;
    TensorRTCvCoreFramework framework(config.c_str());
    FaaOCRProcessor processor;
    auto ctx=TensorRTCvCtx();
    auto output=TensorRTCudaData();

    framework.CreateEngine();
    framework.CreateCtx(ctx);
    std::vector<int64_t> x_layer = {1,3,640,640};
    TensorRTCvCoreFramework::RegisterInputLayer(ctx,"x",x_layer.data(),x_layer.size());
    TensorRTCvCoreFramework::RegisterOutputLayer(ctx,"fetch_name_0",DataType::FLOAT32);

    TensorRTCvCoreFramework::PreProcess(processor,img,blob);
    TensorRTCvCoreFramework::PostImg("x",ctx,blob.data,blob.total(),DataType::FLOAT32);
    TensorRTCvCoreFramework::Forward(ctx);
    TensorRTCvCoreFramework::GetOutput("fetch_name_0",ctx,output,DataType::FLOAT32);
    TensorRTCvCoreFramework::FreeLayer(ctx,"fetch_name_0");
    TensorRTCvCoreFramework::FreeLayer(ctx,"x");
    auto host_float=static_cast<float*>(output.data);
    std::vector<cv::Rect> rects;
    TensorRTCvCoreFramework::PostProcess(processor,host_float,img,blob,rects);
    cv::imwrite("test_v1.png",img);
}

auto run_test2(){
    using faa::fdnn::fcv::FaaCvDnnForTensorRT;
    using faa::fdnn::rt::FaaDnnExecutor;
    using faa::fdnn::fcv::FaaDetCtx;
    using faa::fdnn::fcv::FaaDetOutputData;
    using faa::fdnn::fcv::processer::ocr::FaaOcrDetProc;
    using faa::fdnn::fcv::processer::ocr::FaaDetPreParam;
    using faa::fdnn::rt::code::FaaDnnRtCode;
    using faa::fdnn::fcv::processer::ocr::DetResult;
    using faa::fdnn::fcv::processer::ocr::FaaDetPostParam;
    cv::Mat blob;
    cv::Mat img=cv::imread("test1.png",cv::IMREAD_COLOR);


    FaaDnnExecutor<FaaCvDnnForTensorRT,FaaDetCtx> executor(
        std::make_unique<FaaCvDnnForTensorRT>("./config/trt_ocr.json"));

    auto proc=FaaOcrDetProc();
    auto ctx=FaaDetCtx();
    auto output=FaaDetOutputData();
    auto code=FaaDnnRtCode::Success;
    auto result=DetResult();

    executor.initEngine();
    executor.initCtx(ctx);

    executor.setInput("x",{1,3,640,640},      ctx);
    executor.setOutput("fetch_name_0",DataType::FLOAT32,ctx).
             preProcess(proc,
                 img,blob,FaaDetPreParam{
                    .width = 640,
                    .height = 640,
                    .scalefactor = 1/255.0
                },code);

    executor.postImage("x",blob,ctx,DataType::FLOAT32);
    executor.forward(ctx)
          .getOutput("fetch_name_0",output,ctx,DataType::FLOAT32)
          .postProcess(proc,static_cast<float*>(output.toHost()),
            img,FaaDetPostParam{
                .width = 640,
                .height = 640,
                .threshold    = 0.01,
                .unclip_ratio = 2.0f,
                .PAD = 5,
            },code,result);


    cv::imwrite("test_v4.png",img);
    std::cout<<result.toString()<<std::endl;

    // auto host_float=static_cast<float*>(output.toHost());
    // auto len=output.toLength();
    //
    // std::cout<<"len:"<<len<<std::endl;


}
int main(){
    cv::Mat img = cv::imread("./test1.png");
    // run_test("./config/trt_ocr.json",img);
    run_test2();
    return 0;

}