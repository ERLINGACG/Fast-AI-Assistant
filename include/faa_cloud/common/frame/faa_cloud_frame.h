//
// Created by HP on 2026/8/31.
//

#ifndef FASTSCREEN_FAA_CLOUD_FRAME_H
#define FASTSCREEN_FAA_CLOUD_FRAME_H
#include <mutex>
#include <vector>
#include <opencv2/core/mat.hpp>

namespace faa::cloud::frame{

    struct Frames{

        std::mutex frame_lock;
        std::vector<cv::Mat> frames;

        auto push(const cv::Mat& frame){
            std::lock_guard lock(frame_lock);
            frames.push_back(frame);
        }

        auto peek(cv::Mat& frame) {
            std::lock_guard lock(frame_lock);
            frame=frames.back();
        }

        auto keep(int32_t max){
            std::lock_guard lock(frame_lock);
            if (frames.size()>max){
                frames.erase(frames.begin());
            }
        }

    };

}
#endif //FASTSCREEN_FAA_CLOUD_FRAME_H