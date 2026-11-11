//
// Created by HP on 2026/10/31.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_BACKEND_H
#define FASTAIASSISTANT_ROOT_FAA_BACKEND_H
#include <variant>
#include "faa_core/fdnn/backend/trt_cv/faa_cv_dnn_trt.h"
#include "faa_core/faa_export.h"
#include <nlohmann/json.hpp>
namespace faa::core::fdnn::backend {


    template <typename B, typename C>
    class FAA_CPP_EXPORT FaaBackend {

        // info::log::Ilog* logger=nullptr;
        common::ilogger::FaaIlogger* ilogger=nullptr;

        nlohmann::json config{};
        std::shared_ptr<B> backend;
        C ctx;


    public:
        FaaBackend();
        // void setLogger(info::log::Ilog* logger);
        void setLogger(common::ilogger::FaaIlogger* ilogger);
        void setConfig(nlohmann::json& config);
        void getEngineInfo();

        void initBackend(std::shared_ptr<B> backend);
        void initBackend();

        void initCtx();
        void initCtx(C& ctx);

        template <typename... Args>
        void setInputLayerShape(std::string layer_name,std::vector<int64_t> shape , Args&&... args);

        template <typename... Args>
        void setOutputLayerShape(std::string layer_name,std::string type, Args&&... args);

        template <typename... Args>
        void uploadBlob(std::string layer_name, frame::FaaMat& blob, Args&&... args);

        template <typename... Args>
        void forward(Args&&... args);

        template <typename... Args>
        void downloadBlob(std::string layer_name, frame::FaaMat& output,std::string type, Args&&... args);

        C& getCtx();
        std::shared_ptr<B>&  getBackend();
        std::vector<int64_t> getLayerShape(std::string layer_name);
    };

}
#include "faa_backend_impl.h"
#endif //FASTAIASSISTANT_ROOT_FAA_BACKEND_H