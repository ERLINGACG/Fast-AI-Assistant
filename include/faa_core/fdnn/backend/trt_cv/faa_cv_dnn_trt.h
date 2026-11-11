//
// Created by HP on 2026/10/31.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_H
#define FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_H
#include <memory>
#include <nlohmann/json.hpp>

#include "faa_cv_dnn_trt_ctx.h"
#include "faa_common/ilogger/faa_ilogger.h"
#include "faa_core/faa_export.h"
#include "info/ilog/ilog.h"
#include "faa_core/frame/faa_mat.h"
namespace faa::core::fdnn::backend::trt {

    struct TensorRTFrameworkImpl;

    class FAA_CPP_EXPORT FaaCvTensorRtBackend {

        std::unique_ptr<TensorRTFrameworkImpl> engine;
        // info::log::Ilog* logger = nullptr;
        common::ilogger::FaaIlogger* ilogger = nullptr;


        nlohmann::json config{};

    public:

        FaaCvTensorRtBackend();
        ~FaaCvTensorRtBackend();

        FaaCvTensorRtBackend( FaaCvTensorRtBackend&& other) noexcept;
        FaaCvTensorRtBackend(const FaaCvTensorRtBackend&) = delete;
        FaaCvTensorRtBackend& operator=(FaaCvTensorRtBackend&& other) noexcept;

        // void setLogger(info::log::Ilog* logger);
        void setLogger(common::ilogger::FaaIlogger* ilogger);

        void setConfig(nlohmann::json& config);

        void getEngineInfo();
        void initBackend();
        void initCtx(FaaCvTensorRTCtx& ctx) ;
        void setInputLayerShape (FaaCvTensorRTCtx& ctx, const std::string& layer_name, std::vector<int64_t> shape) const;
        void setInputLayerShape (FaaCvTensorRTCtx& ctx, const std::string& layer_name, std::vector<int64_t> shape,const std::string& type) const;
        void setOutputLayerShape(FaaCvTensorRTCtx& ctx, const std::string& layer_name, const std::string& type) const;
        void uploadBlob         (FaaCvTensorRTCtx& ctx, const std::string& layer_name, const frame::FaaMat& input) const;
        void downloadBlob       (FaaCvTensorRTCtx& ctx, const std::string& layer_name, const frame::FaaMat& output,const std::string& type) const;
        void forward            (FaaCvTensorRTCtx& ctx) const;

        std::vector<int64_t> getLayerShape(FaaCvTensorRTCtx& ctx,const std::string& layer_name) const;



    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_CV_DNN_TRT_H