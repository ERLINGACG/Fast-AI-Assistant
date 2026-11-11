//
// Created by HP on 2026/10/31.
//
#include <opencv2/imgcodecs.hpp>
#include <utility>

#include "faa_core/fdnn/backend/trt_cv/faa_cv_dnn_trt.h"
#include "faa_core/fdnn/backend/trt_cv/private/faa_cv_trt_ctx_impl.h"
#include "faa_core/fdnn/backend/trt_cv/private/faa_cv_trt_framework_impl.h"
#include "faa_core/fdnn/backend/trt_cv/private/faa_cv_trt_type_map.h"
#include "faa_core/frame/private/faa_mat_impi.h"
using faa::core::frame::FaaMat;
using faa::core::frame::FaaMatImpl;
using faa::core::fdnn::backend::trt::FaaCvTensorRtBackend;
using faa::core::fdnn::backend::trt::TensorRTFrameworkImpl;
using faa::core::fdnn::backend::trt::type_map;



void FaaCvTensorRtBackend::setInputLayerShape(FaaCvTensorRTCtx& ctx, const std::string& layer_name,
    std::vector<int64_t> shape) const {
    gdlz::trt::icv::core::TensorRTCvCoreFramework::ResetInputLayer(
            ctx.getCtx().ctx,layer_name.c_str(),
        shape.data(),
        static_cast<int64_t>(shape.size())
    );
}

void FaaCvTensorRtBackend::setInputLayerShape(FaaCvTensorRTCtx& ctx, const std::string& layer_name,
    std::vector<int64_t> shape, const std::string& type) const
{
    auto it=type_map_v2.find(type);
    if(it==type_map_v2.end()){
        if (ilogger) ilogger->error("type not found");
        return;
    }
    gdlz::trt::icv::core::TensorRTCvCoreFramework::ResetInputLayer(
        ctx.getCtx().ctx,layer_name.c_str(),
        shape.data(),
        static_cast<int64_t>(shape.size()),
        it->second
    );
}

void FaaCvTensorRtBackend::setOutputLayerShape(FaaCvTensorRTCtx& ctx, const std::string& layer_name,
                                               const std::string& type) const {
    auto it=type_map_v2.find(type);
    if(it==type_map_v2.end()){
        if (ilogger) ilogger->error("type not found");
        return;
    }
    gdlz::trt::icv::core::TensorRTCvCoreFramework::ResetOutputLayer(
        ctx.getCtx().ctx,layer_name.c_str(),
        it->second
    );
}

void FaaCvTensorRtBackend::uploadBlob(FaaCvTensorRTCtx& ctx, const std::string& layer_name,
    const frame::FaaMat& input) const
{
    auto& mat = input.impl()->mat;
    const auto* src = reinterpret_cast<const std::byte*>(mat.data);
    const std::vector bytes(src, src + mat.total() * mat.elemSize());
    gdlz::trt::icv::core::TensorRTCvCoreFramework::UploadData(
        layer_name.c_str(),
        ctx.getCtx().ctx,
        bytes
    );
}

void FaaCvTensorRtBackend::downloadBlob(FaaCvTensorRTCtx& ctx, const std::string& layer_name, const frame::FaaMat& output,
    const std::string& type) const
{
    using gdlz::trt::icv::core::TensorRTCvCoreFramework;
    TensorRTCvCoreFramework::DownloadData(
        layer_name.c_str(),
        ctx.getCtx().ctx
    );
    const auto& buffer = TensorRTCvCoreFramework::GetBuffer(layer_name.c_str(), ctx.getCtx().ctx);
    const int elems = static_cast<int>(buffer.size() / sizeof(float));
    // 零拷贝：mat 直接引用 buffer 的 host 内存
    output.impl()->mat = cv::Mat(1, elems, CV_32F, buffer.host());

}

void FaaCvTensorRtBackend::forward(FaaCvTensorRTCtx& ctx) const
{
    gdlz::trt::icv::core::TensorRTCvCoreFramework::Forward(
        ctx.getCtx().ctx
    );
}

std::vector<int64_t> FaaCvTensorRtBackend::getLayerShape(FaaCvTensorRTCtx& ctx, const std::string& layer_name) const {
    return gdlz::trt::icv::core::TensorRTCvCoreFramework::GetLayerShape(layer_name.c_str(), ctx.getCtx().ctx);
}
