//
// Created by HP on 2026/10/31.
//
#include "faa_core/fdnn/backend/trt_cv/faa_cv_dnn_trt.h"
#include "faa_core/fdnn/backend/trt_cv/private/faa_cv_trt_framework_impl.h"
#include "faa_core/fdnn/backend/trt_cv/private/faa_cv_trt_ctx_impl.h"
using faa::core::fdnn::backend::trt::FaaCvTensorRtBackend;
using faa::core::fdnn::backend::trt::TensorRTFrameworkImpl;
using faa::core::fdnn::backend::trt::FaaCvTensorRTCtxImpl;


void FaaCvTensorRtBackend::initCtx(FaaCvTensorRTCtx& ctx) {

    auto model_config_path = config.value("model_config_path", "");
    if (model_config_path.empty()) {
        if (ilogger) ilogger->error("model_config_path is empty");
        return;
    }
    engine->getFramework().CreateCtx(ctx.getCtx().ctx);
    if (!engine->isInit()) {
        if (ilogger) ilogger->error("init model failed: {}", model_config_path);
        return;
    }
    // std::cout<<"init model success: "<<model_config_path<<std::endl;
    if (ilogger) ilogger->debug("init model success: {}", model_config_path);

}
void FaaCvTensorRtBackend::getEngineInfo() {
    engine->getFramework().GetEngineInfo();
}

void FaaCvTensorRtBackend::initBackend()
{
    auto model_config_path = config.value("model_config_path", "");
    if (model_config_path.empty()) {
        if (ilogger) ilogger->error("model_config_path is empty");
        return;
    }
    engine->init(model_config_path);
}
