//
// Created by HP on 2026/10/19.
//

#include "faa_core/feature/faa_cv_hough_circle.h"
#include <algorithm>

namespace faa::core::fcv::feature {

    FaaCircleResult houghCircleDetect(
        const cv::Mat& input,
        const FaaHoughCircleParam& param) {

        FaaCircleResult result;
        cv::Mat gray;

        CV_Assert(!input.empty());
        if (input.channels() == 1) {
            if (input.depth() != CV_8U) {
                double mn, mx;
                cv::minMaxLoc(input, &mn, &mx);
                input.convertTo(gray, CV_8U, 255.0 / (mx - mn + 1e-6), -mn * 255.0 / (mx - mn + 1e-6));
            } else {
                gray = input.clone();
            }
        } else if (input.channels() == 3) {
            cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
        } else if (input.channels() == 4) {
            cv::cvtColor(input, gray, cv::COLOR_BGRA2GRAY);
        }
        cv::GaussianBlur(gray, gray, cv::Size(5, 5), 1.5);

        std::vector<cv::Vec3f> circles;
        cv::HoughCircles(
            gray, circles,
            cv::HOUGH_GRADIENT,
            param.dp, param.min_dist,
            param.param1, param.param2,
            param.min_radius, param.max_radius);

        result.circles.reserve(circles.size());
        for (const auto& c : circles) {
            result.circles.push_back({
                static_cast<int32_t>(c[0]),
                static_cast<int32_t>(c[1]),
                static_cast<int32_t>(c[2]),
                c[2]
            });
        }
        return result;
    }

    void houghCircleNMS(
        FaaCircleResult& result,
        double overlap_threshold) {

        auto& circles = result.circles;
        std::sort(circles.begin(), circles.end(), [](const auto& a, const auto& b) {
            return a.score > b.score;
        });

        std::vector<bool> suppressed(circles.size(), false);
        std::vector<FaaCircleResult::CircleInfo> kept;
        kept.reserve(circles.size());

        for (size_t i = 0; i < circles.size(); ++i) {
            if (suppressed[i]) continue;
            kept.push_back(circles[i]);

            for (size_t j = i + 1; j < circles.size(); ++j) {
                if (suppressed[j]) continue;
                const auto& a = circles[i];
                const auto& b = circles[j];

                double dist = std::hypot(a.x - b.x, a.y - b.y);
                double min_r = std::min(a.radius, b.radius);

                if (dist < min_r * overlap_threshold * 2) {
                    suppressed[j] = true;
                }
            }
        }
        circles = std::move(kept);
    }

} // namespace faa::core::fcv::feature