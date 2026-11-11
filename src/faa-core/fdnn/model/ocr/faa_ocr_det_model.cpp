//
// Created by HP on 2026/11/3.
//
#include "faa_core/fdnn/model/ocr/faa_ocr_det_model.h"

#include "faa_core/fdnn/backend/trt_cv/data/faa_cv_dnn_trt_type.h"
#include "faa_core/fdnn/general/ocr/faa_cv_ocr_det_pros.h"
using faa::core::fdnn::model::ocr::FaaOcrDetModel;
using faa::core::fdnn::backend::trt::kFaaFloat32;
using faa::core::frame::FaaMat;
// void FaaOcrDetModel::init() {
//
// }
void FaaOcrDetModel::setConfig(const nlohmann::json& config)
{

    FaaDnnModelBase::setConfig(config);
    const auto means    = this->config.value("mean",nlohmann::json::object());
    const auto img_size = this->config.value("img_size",nlohmann::json::object());
    auto x              = this->config.value("import_layer",nlohmann::json::object());
    auto fetch_name_0   = this->config.value("exit_layer",nlohmann::json::object());

    if (ilogger) ilogger->debug("\n the x is {}",x.dump(2));
    if (ilogger) ilogger->debug("the fetch_name_0 is {}",fetch_name_0.dump(2));

    this->mean.b        = means.value("b",0.0);
    this->mean.g        = means.value("g",0.0);
    this->mean.r        = means.value("r",0.0);
    this->img_h         = img_size.value("h",640);
    this->img_w         = img_size.value("w",640);
    this->scalefactor   = this->config.value("scale",1.0);

    layer::build_InputLayer_to_json(this->x,x);
    layer::build_OutputLayer_to_json(this->fetch_name_0,fetch_name_0);

}
void FaaOcrDetModel::run(
    FaaMat& img,
    const FaaDetRunParam& param,
    general::ocr::FaaDetResults& results
    )
{
    try {
        std::visit([this, &img, &param, &results]<typename T0>(T0&& tag) {

             using T = std::decay_t<T0>;
             if constexpr (std::is_same_v<T, backend::kTensorRT>){
                 run0(img, param, results);
             }
             if constexpr (std::is_same_v<T, backend::kOnnxRT>) {
                 if (ilogger) ilogger->info("the backend is onnxRT, but is not implemented");
                 throw std::runtime_error("ONNX backend not implemented");
             }

        }, this->type);
    } catch (const std::exception& e) {
       if (ilogger) ilogger->error("run ocr det model failed: {}",e.what());
    }
}

void FaaOcrDetModel::run0(
    frame::FaaMat& img,
    const FaaDetRunParam& param,
    general::ocr::FaaDetResults& results
) {

    FaaMat blob;
    FaaMat output;

    general::blob::setBlob(
        blob,
        img,
        general::blob::FaaBlobParam{
            .mean        = this->mean,
            .h           = this->img_h,
            .w           = this->img_w,
            .scalefactor = this->scalefactor,
            .swapRB = false,
            .crop   = false,
        }
    );
    getBackend0().setInputLayerShape  (x.name,x.opt_shape);
    getBackend0().setOutputLayerShape (
        fetch_name_0.name, fetch_name_0.dtype
    );
    getBackend0().uploadBlob(x.name,blob);
    getBackend0().forward();
    getBackend0().downloadBlob(
        fetch_name_0.name,
        output,
        fetch_name_0.dtype
    );
    general::ocr::detPostProcess(
        output,
        img,
        this->img_w,
        this->img_h,
        param.param,
        results
    );
    if (param.isSave){
        general::ocr::drawDetResults(img, results, true);
    }
}
