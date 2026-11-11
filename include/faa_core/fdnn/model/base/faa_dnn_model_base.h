//
// Created by HP on 2026/11/3.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_DNN_MODEL_BASE_H
#define FASTAIASSISTANT_ROOT_FAA_DNN_MODEL_BASE_H

#include <nlohmann/json.hpp>

#include "faa_common/ilogger/faa_ilogger.h"
#include "faa_core/fdnn/backend/faa_backend_type.h"
#include "faa_core/fdnn/backend/faa_backend.h"
#include "faa_core/fdnn/backend/trt_cv/faa_cv_dnn_trt.h"
#include "faa_core/fdnn/backend/trt_cv/faa_cv_dnn_trt_ctx.h"
namespace faa::core::fdnn::model::base {



    class FAA_CPP_EXPORT FaaDnnModelBase {

        using TensorRTBackend = backend::FaaBackend<
                backend::trt::FaaCvTensorRtBackend,
                backend::trt::FaaCvTensorRTCtx
        >;
    protected:

        backend::FaaDnnBackEndType type;
        // info::log::Ilog* logger              = nullptr;
        common::ilogger::FaaIlogger* ilogger = nullptr;
        nlohmann::json config {};
        TensorRTBackend backend_0 ;


    public:

        void setBackend(const backend::FaaDnnBackEndType type) { this->type = type; }
        void setBackend(const std::string& type);
        void printInfo() ;
        void initBackend();
        void initCtx();
        void setConfig(const nlohmann::json& config);
        // void setLogger(info::log::Ilog* logger);
        void setLogger(common::ilogger::FaaIlogger* ilogger);
        void shapeInit(FaaDnnModelBase& provider);

        // [[nodiscard]] auto* getLogger() const { return logger; }
        [[nodiscard]] auto& getConfig() const { return config; }
        [[nodiscard]] bool checkBackend(backend::FaaDnnBackEndType type) const;
        [[nodiscard]] bool checkBackend0 () const { return checkBackend(backend::kTensorRT{}); } ;

        TensorRTBackend& getBackend0() {  return backend_0; }


    };
}
#include "./faa_dnn_model_base_impl.h"
#endif //FASTAIASSISTANT_ROOT_FAA_DNN_MODEL_BASE_H