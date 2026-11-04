//
// Created by HP on 2026/10/16.
//

#ifndef FASTSCREEN_FAA_CV_SURF_FEATURE_H
#define FASTSCREEN_FAA_CV_SURF_FEATURE_H
#include "faa_core/resources/faa_cv_resources.h"
#include "info/ilog/ilog.h"
#include "faa_core/faa_export.h"
namespace faa::core::fcv::feature {

    struct FaaSurfParam {
        std::string name;
        std::string global_name;

        float hessianThreshold = 400.0f;
        int nOctaves = 4;
        int nOctaveLayers = 2;
        bool extended = false;
        float keypointsRatio = 0.01f;
        bool upright = false;

        auto build(const nlohmann::json& config) {

            this->name             = config.value("name", "");
            this->global_name      = config.value("global_name", "");
            this->hessianThreshold = config.value("hessian_threshold", 0.0f);
            this->nOctaves         = config.value("nOctaves", 4);
            this->nOctaveLayers    = config.value("nOctaveLayers", 2);
            this->extended         = config.value("extended", false);
            this->keypointsRatio   = config.value("keypointsRatio", 1.0f);
            this->upright          = config.value("upright", false);
            return *this;
        }
    };

    struct FaaSurfResult {

        struct Item{
            int32_t x;
            int32_t y;
            int32_t w;
            int32_t h;
            float score;
        } item ;

        std::vector<Item> items;

        auto serialize() {
            auto json = nlohmann::json::array();
            for (auto& item : this->items) {
                auto obj = nlohmann::json::object();
                obj["x"]     = item.x;
                obj["y"]     = item.y;
                obj["w"]     = item.w;
                obj["h"]     = item.h;
                obj["score"] = item.score;
                json.push_back(obj);
            }
            return json;
        }
    };

    class FAA_CPP_EXPORT FaaCVSurfFeature {

        protected:
            info::log::Ilog* logger;
            // std::unique_ptr<resources::FaaCvResource> resource;

        public:
            FaaCVSurfFeature();
            ~FaaCVSurfFeature() = default ;

            void setLogger(info::log::Ilog* logger);
            void runSurfToGPU(
                const cv::Mat& image,
                const FaaSurfParam& param,
                resources::FaaCvResource& resource,
                FaaSurfResult& result) const;


    };
}
#endif //FASTSCREEN_FAA_CV_SURF_FEATURE_H