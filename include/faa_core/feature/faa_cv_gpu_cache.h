//
// Created by HP on 2026/10/16.
//

#ifndef FASTSCREEN_FAA_CV_GPU_CACHE_H
#define FASTSCREEN_FAA_CV_GPU_CACHE_H
#include <string>
#include <opencv2/opencv.hpp>

namespace faa::core::fcv::feature {

    struct  FaaCvGPUcacheItem {
        std::string name;
        std::string global_name;

        cv::cuda::GpuMat gpu_tmpl_kpts; // 模板图像的特征点
        cv::cuda::GpuMat gpu_tmpl_desc; // 模板图像的特征描述子
    };

    struct  FaaCvGPUcache {

        std::map<std::string, std::vector<FaaCvGPUcacheItem>> items;
        std::mutex items_mutex;

        auto push(const std::string& global_name, const FaaCvGPUcacheItem& item) {
            std::lock_guard lock(items_mutex);
            items[global_name].push_back(item);
        }

        auto query(const std::string& global_name, const std::string& name) {
            std::lock_guard lock(items_mutex);
            if (!items.contains(global_name)) return {};

            for (auto& item : items[global_name]) {
                if (item.name == name) {
                    return item;
                }
            }
            return {};
        }

    };
}
#endif //FASTSCREEN_FAA_CV_GPU_CACHE_H