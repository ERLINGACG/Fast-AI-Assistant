//
// Created by HP on 2026/8/5.
//

#ifndef FASTSCREEN_FAA_SERVICE_RESOURCES_FRAMES_H
#define FASTSCREEN_FAA_SERVICE_RESOURCES_FRAMES_H
#include <map>
#include <mutex>
#include <vector>
#include <opencv2/core/mat.hpp>

namespace faa::service::resources::frame{
    struct FrameResources{
        int32_t max_frames=3;
        std::mutex frame_mutex;
        std::map<std::string,std::vector<cv::Mat>> frames;

        auto push(const std::string& global_name, const cv::Mat& frame){
            std::lock_guard lock(frame_mutex);
            frames[global_name].push_back(frame);
            if(frames[global_name].size()>max_frames){
                frames[global_name].erase(frames[global_name].begin());
            }
            return true;
        }
        auto peek(const std::string& global_name,cv::Mat& frame){
            std::lock_guard lock(frame_mutex);
            if (!frames.contains(global_name)){
                return false;
            }
            frame = frames[global_name].back();
            return true;
        }
    };
}
#endif //FASTSCREEN_FAA_SERVICE_RESOURCES_FRAMES_H