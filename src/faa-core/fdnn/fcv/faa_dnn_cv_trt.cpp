//
// Created by HP on 2026/7/9.
//
#include "faa_core/fdnn/fcv/faa_dnn_cv_trt.h"

#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn/dnn.hpp>

#include "faa_core/fdnn/data/faa_dnn_data_type.h"


using faa::fdnn::fcv::FaaCvDnnForTensorRT;
using gdlz::trt::icv::core::TensorRTCvCoreFramework;
FaaCvDnnForTensorRT::FaaCvDnnForTensorRT(const std::string& config_path){
    this->framework=std::make_unique<TensorRTCvCoreFramework>(config_path.c_str());
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::setInput(const std::string& name, std::vector<int64_t> shape, FaaCvCtx& ctx){
    // TensorRTCvCoreFramework::RegisterInputLayer(
    //     ctx.ctx,
    //     name.c_str(),
    //     shape.data(),
    //     static_cast<int64_t>(shape.size()));
    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::setOutput(const std::string& name, gdlz::trt::core::cuda::DataType dataType,
    FaaCvCtx& ctx) {
    // TensorRTCvCoreFramework::RegisterOutputLayer(
    //     ctx.ctx,
    //     name.c_str(),
    //     dataType);
    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::initEngine(){
    // framework->CreateEngine();
    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::initCtx(FaaCvCtx& ctx) {
    // framework->CreateCtx(ctx.ctx);
    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::resetInput(const std::string& name, const std::vector<int64_t>& shape, FaaCvCtx& ctx)
{

    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::resetOutput(const std::string& name, FaaCvCtx& ctx,
    gdlz::trt::core::cuda::DataType dataType)
{

    return *this;
}


FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::postImage(
    const std::string& name, cv::Mat& img,
    FaaCvCtx& ctx,gdlz::trt::core::cuda::DataType dataType
    ) {


    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::clear(FaaCvCtx& ctx) {

    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::forward(FaaCvCtx& ctx){

    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::getOutput(const std::string& name,FaaCvOutputData& output, FaaCvCtx& ctx,gdlz::trt::core::cuda::DataType dataType){


    return *this;
}

FaaCvDnnForTensorRT& FaaCvDnnForTensorRT::getOutput(
    const std::string& name, FaaCvOutputData& output, FaaCvCtx& ctx,
    const std::string& dataType,
    info::log::Ilog* ilog
    )
{
    return *this;
}

std::vector<int64_t> FaaCvDnnForTensorRT::getOutputShapes(const std::string& name, FaaCvCtx& ctx)
{
    std::vector<int64_t> shape;
    return shape;
}



