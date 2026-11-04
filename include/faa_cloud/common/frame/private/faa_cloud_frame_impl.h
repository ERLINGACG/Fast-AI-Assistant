//
// Created by HP on 2026/10/21.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CLOUD_FRAME_IMPI_H
#define FASTAIASSISTANT_ROOT_FAA_CLOUD_FRAME_IMPI_H
#include <mutex>
#include <opencv2/core/mat.hpp>


namespace faa::cloud::frame {


    struct FramesImpl {

        std::mutex frame_lock;
        std::vector<cv::Mat> frames;

        auto peek(cv::Mat& frame) {
            std::lock_guard lock(frame_lock);
            if (frames.empty()){
                return;
            }
            frame=frames.back();
        }

        auto keep(const int32_t max){
            std::lock_guard lock(frame_lock);
            if (frames.size()>max){
                frames.erase(frames.begin());
            }
        }

        auto push(const cv::Mat& frame){
            std::lock_guard lock(frame_lock);
            frames.push_back(frame);
        }





    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_CLOUD_FRAME_IMPI_H