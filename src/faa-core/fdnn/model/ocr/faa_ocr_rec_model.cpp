//
// Created by HP on 2026/11/4.
//
#include "faa_core/fdnn/model/ocr/faa_ocr_rec_model.h"

#include "faa_core/fdnn/backend/faa_backend_type_oper.h"
#include "faa_core/fdnn/general/ocr/faa_cv_ocr_rec_pros.h"
#include "faa_core/frame/private/faa_mat_impi.h"
using faa::core::fdnn::model::ocr::FaaOcrRecModel;

void FaaOcrRecModel::loadVocab(const std::string& vocab_path) {
    this->vocal_file.open(vocab_path);
    if (!this->vocal_file.is_open()) {
        throw std::runtime_error("open vocab file failed");
    }
    std::string line;
    while (std::getline(this->vocal_file, line)) {
        this->vocab.push_back(line);
    }
    if (ilogger) ilogger->debug("the vocab size is {}",this->vocab.size());
}

void FaaOcrRecModel::loadVocab() {

    this->loadVocab(this->config.value("vocab_path",""));
}

void FaaOcrRecModel::setConfig(const nlohmann::json& config) {

    FaaDnnModelBase::setConfig(config);

    const auto means    = this->config.value("mean",nlohmann::json::object());
    const auto img_size = this->config.value("img_size",nlohmann::json::object());
    auto x              = this->config.value("import_layer",nlohmann::json::object());
    auto fetch_name_0   = this->config.value("exit_layer",nlohmann::json::object());

    if (ilogger) ilogger->debug("the x is {}",x.dump(2));
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

void FaaOcrRecModel::run(
    frame::FaaMat& org,
    general::ocr::FaaRecParam& param,
    general::ocr::FaaDetResults& det_results,
    general::ocr::FaaRecResults& result
    ) {
    backend::visit_to_type(
        this->type,
        [this, &org, &param, &det_results, &result]() {

            this->run0(org,param,det_results,result);
        },
        [this]() {
            if (ilogger) ilogger->info("the backend is onnxRT, but is not implemented");
            throw std::runtime_error("ONNX backend not implemented");
        }
    );
}

void FaaOcrRecModel::run0(
    frame::FaaMat& org,
    general::ocr::FaaRecParam& param,
    general::ocr::FaaDetResults& det_results,
    general::ocr::FaaRecResults& result

    )
{
    const auto& mat = org.impl()->mat;
    if (
        this->x.min_shape.size() != 4 ||
        this->x.max_shape.size() != 4 ||
        this->x.opt_shape.size() != 4
    ) {
        throw std::runtime_error("the shape is not 4 dim");
    }
    auto min_w = this->x.min_shape[3];
    auto max_w = this->x.max_shape[3];
    auto opt_w = this->x.opt_shape[3];

    auto h      = this->x.opt_shape[2];
    auto proc_batch  = this->x.opt_shape[0];
    auto proc_channel= this->x.opt_shape[1];

    for (auto& rec_box : det_results.items) {

        constexpr int kStride = 32;
        auto roi    = frame::FaaMat();
        auto blob   = frame::FaaMat();
        auto output = frame::FaaMat();

        cv::Rect rect(rec_box.x, rec_box.y, rec_box.w, rec_box.h);
        roi.impl()->mat = mat(rect);

        // ✅ Step1:保持 ROI 原始长宽比 → 算目标宽度
        const double aspect = roi.impl()->mat.cols / static_cast<double>(roi.impl()->mat.rows);
        int w = static_cast<int>(
            static_cast<double>(h) * aspect
        );

        // ✅ Step2:对齐到 stride(32) → 否则 CNN 下采样后特征图宽度和 RNN 序列长度对不上
        w = (w / kStride) * kStride;
        // ✅ Step3:夹到 profile 合法区间
        w = std::max(w, static_cast<int>(min_w));
        w = std::min(w, static_cast<int>(max_w));

        general::blob::setBlob(blob,roi,{
            .mean =  this -> mean,
            .h           = static_cast<int>(h),
            .w           = w,
            .scalefactor = this->scalefactor,
            .swapRB = false,
            .crop   = false,
        });
        getBackend0().setInputLayerShape  (x.name,{proc_batch,proc_channel,h,w});
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

        auto output_shape = getBackend0().getLayerShape(fetch_name_0.name);
        std::string text;
        param.seq_len          = static_cast<int>(output_shape[1]);
        param.model_vocal_size = static_cast<int>(output_shape[2]);

        general::ocr::recPostProcess(
            output,
            roi,
            param,
            this->vocab,
            text
        );

        result.items.push_back({
            text,
            rec_box.score,
            rec_box.x,
            rec_box.y,
            rec_box.w,
            rec_box.h}
        );

    }

}
