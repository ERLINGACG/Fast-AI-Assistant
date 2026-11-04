//
// Created by HP on 2026/10/19.
//

#ifndef FASTSCREEN_FAA_CV_HOUGH_CIRCLE_H
#define FASTSCREEN_FAA_CV_HOUGH_CIRCLE_H
#include "faa_core/faa_export.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <nlohmann/json.hpp>

namespace faa::core::fcv::feature {

    struct FaaHoughCircleParam {
        double dp          = 1.0;
        double min_dist    = 30.0;
        double param1      = 100.0;
        double param2      = 50.0;
        int    min_radius  = 5;
        int    max_radius  = 300;

        auto& build(const nlohmann::json& j){
            dp          = j.value("dp", dp);
            min_dist    = j.value("min_dist", min_dist);
            param1      = j.value("param1", param1);
            param2      = j.value("param2", param2);
            min_radius  = j.value("min_radius", min_radius);
            max_radius  = j.value("max_radius", max_radius);
            return *this;
        }
    };

    struct FaaCircleResult {
        struct CircleInfo {
            int32_t x, y, radius;
            float   score;
        };
        std::vector<CircleInfo> circles;

        [[nodiscard]] size_t size() const  { return circles.size(); }
        [[nodiscard]] bool   empty() const { return circles.empty();}

        [[nodiscard]] auto serialize() const {
            auto j = nlohmann::json::object();
            j["circles"] = nlohmann::json::array();
            for (const auto& [x, y, radius, score] : circles) {
                j["circles"].push_back(
                    {x, y, radius, score}
                    );
            }
            return j;
        };
    };

    FAA_CPP_EXPORT FaaCircleResult houghCircleDetect(
        const cv::Mat& input,
        const FaaHoughCircleParam& param);

    FAA_CPP_EXPORT void houghCircleNMS(
        FaaCircleResult& result,
        double overlap_threshold = 0.5);

} // namespace faa::core::fcv::feature

#endif //FASTSCREEN_FAA_CV_HOUGH_CIRCLE_H