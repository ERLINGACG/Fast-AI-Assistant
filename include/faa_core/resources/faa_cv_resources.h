//
// Created by HP on 2026/10/16.
//

#ifndef FASTSCREEN_FAA_CV_RESOURCES_H
#define FASTSCREEN_FAA_CV_RESOURCES_H
#include <map>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <opencv2/imgcodecs.hpp>

#include <opencv2/core/mat.hpp>

#include "info/ilog/ilog.h"
#include "faa_core/faa_export.h"

namespace faa::core::fcv::resources {

    struct FaaCvResItem {
        std::string name;
        std::string path;

        cv::Mat image;
        cv::Mat float_vec;

        auto& read() {
            image = cv::imread(path);
            return *this;
        }
    };

    struct FAA_CPP_EXPORT FaaCvResource {

        nlohmann::json res_config{};
        std::map<std::string,std::vector<FaaCvResItem>> data;


        // auto& load() ;
        FaaCvResource& load(info::log::Ilog* ilog = nullptr);
        FaaCvResource& setConfig(const nlohmann::json& config);
        bool query(const std::string& global_name, const std::string& item_name,cv::Mat& image);

    };






}


#endif //FASTSCREEN_FAA_CV_RESOURCES_H