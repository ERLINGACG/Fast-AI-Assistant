//
// Created by HP on 2026/7/9.
//
#include "faa-core/fdnn/fcv/faa_dnn_cv_trt.h"

#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn/dnn.hpp>


using faa::fdnn::fcv::FaaCvDnnForTensorRT;
using gdlz::trt::icv::core::TensorRTCvCoreFramework;
FaaCvDnnForTensorRT::FaaCvDnnForTensorRT(const std::string& config_path){
    this->framework=std::make_unique<TensorRTCvCoreFramework>(config_path.c_str());
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::setInput(const std::string& name, std::vector<int64_t> shape, FaaCvCtx& ctx){
    TensorRTCvCoreFramework::RegisterInputLayer(
        ctx.ctx,
        name.c_str(),
        shape.data(),
        static_cast<int64_t>(shape.size()));
    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::setOutput(const std::string& name, gdlz::trt::core::cuda::DataType dataType,
    FaaCvCtx& ctx) {
    TensorRTCvCoreFramework::RegisterOutputLayer(
        ctx.ctx,
        name.c_str(),
        dataType);
    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::initEngine(){
    framework->CreateEngine();
    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::initCtx(FaaCvCtx& ctx) {
    framework->CreateCtx(ctx.ctx);
    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::resetInput(const std::string& name, const std::vector<int64_t>& shape, FaaCvCtx& ctx)
{
    TensorRTCvCoreFramework::ResetInputLayer(ctx.ctx,name.c_str(),shape.data(),static_cast<int64_t>(shape.size()));
    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::resetOutput(const std::string& name, FaaCvCtx& ctx,
    gdlz::trt::core::cuda::DataType dataType)
{
    TensorRTCvCoreFramework::ResetOutputLayer(ctx.ctx,name.c_str(),dataType);
    return *this;
}


// FaaOcrForTensorRT& FaaOcrForTensorRT::PreProcess(cv::Mat& img, cv::Mat& blob,const FaaDetParam& param) {
//     cv::dnn::blobFromImage(img,blob,param.scalefactor,
//         cv::Size(param.width,param.height),
//         cv::Scalar(),false,false);
//     return *this;
// }

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::postImage(
    const std::string& name, cv::Mat& img,
    FaaCvCtx& ctx,gdlz::trt::core::cuda::DataType dataType
    ) {

    TensorRTCvCoreFramework::PostImg(name.c_str(),ctx.ctx,img.data,static_cast<int64_t>(img.total()),dataType);
    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::clear(FaaCvCtx& ctx) {
    for(const auto& key : *ctx.ctx.layer_address | std::views::keys){
        TensorRTCvCoreFramework::FreeLayer(ctx.ctx,key.c_str());
    }
    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::forward(FaaCvCtx& ctx){
    TensorRTCvCoreFramework::Forward(ctx.ctx);
    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::getOutput(const std::string& name,FaaCvOutputData& output, FaaCvCtx& ctx,gdlz::trt::core::cuda::DataType dataType){


    TensorRTCvCoreFramework::GetOutput(name.c_str(),ctx.ctx,output.cuda_data,dataType);

    return *this;
}

std::vector<int64_t> FaaCvDnnForTensorRT::getOutputShapes(const std::string& name, FaaCvCtx& ctx)
{
    std::vector<int64_t> shape;
    auto  data_len=TensorRTCvCoreFramework::GetOutputShape(name.c_str(),ctx.ctx);
    auto* data_prt=TensorRTCvCoreFramework::GetOutputShapes(name.c_str(),ctx.ctx);
    shape.assign(data_prt,data_prt+data_len);
    return shape;
}

// FaaOcrForTensorRT& FaaOcrForTensorRT::PostProcess(float* host_float, cv::Mat org_mat, const FaaDetPostParam& param,
//                                                         std::vector<cv::Rect>& rects) {
//
//     rects.clear();
//     cv::Mat probMap(param.height, param.width, CV_32F);
//     cv::Mat(param.height, param.width, CV_32F, host_float).copyTo(probMap);
//     double p_min, p_max;
//     cv::minMaxLoc(probMap, &p_min, &p_max);
//     if (p_max > p_min) {
//         probMap = (probMap - p_min) / (p_max - p_min);
//     }
//     cv::Mat bin;
//     cv::threshold(probMap, bin, param.threshold, 1.0, cv::THRESH_BINARY);
//     bin.convertTo(bin, CV_8U, 255);
//
//     std::vector<std::vector<cv::Point>> contours;
//     std::vector<cv::Vec4i> hierarchy;
//     cv::findContours(bin, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
//
//     const double min_area = std::max(5.0, 30.0 *
//         org_mat.cols * org_mat.rows / (param.width * param.height)
//     );
//     std::vector<cv::RotatedRect> textBoxes;
//     for (const auto& c : contours) {
//         double area = cv::contourArea(c);
//         if (area < min_area) continue;
//         cv::RotatedRect r = cv::minAreaRect(c);
//         // unclip 膨胀：放大宽高，保持中心和角度不变
//         r.size.width  *= param.unclip_ratio;
//         r.size.height *= param.unclip_ratio;
//         textBoxes.push_back(r);
//     }
//     float sx = static_cast<float>(org_mat.cols) / static_cast<float>(param.width);
//     float sy = static_cast<float>(org_mat.rows) / static_cast<float>(param.height);
//
//     for (const auto& rect : textBoxes) {
//         // 1. 映射到原图坐标
//         cv::Point2f center(rect.center.x * sx, rect.center.y * sy);
//         cv::Size2f size(rect.size.width * sx, rect.size.height * sy);
//         cv::RotatedRect scaled_rect(center, size, rect.angle);
//
//         // 2. 取轴对齐包围盒 + 留白，直接从原图裁剪
//         cv::Rect bbox = scaled_rect.boundingRect();
//         bbox.x      = std::max(0, bbox.x - param.PAD);
//         bbox.y      = std::max(0, bbox.y - param.PAD);
//         bbox.width  = std::min(org_mat.cols - bbox.x, bbox.width + 2 * param.PAD);
//         bbox.height = std::min(org_mat.rows - bbox.y, bbox.height + 2 * param.PAD);
//         if (bbox.width <= 0 || bbox.height <= 0) continue;
//         cv::rectangle(org_mat, bbox, cv::Scalar(0, 0, 255), 2);
//         rects.push_back(bbox);
//     }
//     return *this;
// }
