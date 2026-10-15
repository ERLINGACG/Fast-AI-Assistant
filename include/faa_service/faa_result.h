//
// Created by HP on 2026/7/20.
//

#ifndef FASTSCREEN_FAA_RESULT_H
#define FASTSCREEN_FAA_RESULT_H
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include "faa_core/fdnn/fcv/processer/ocr/faa_ocr_det_proc.h"

namespace faa::service{
    struct FaaSerOcrResult{
        std::vector<std::pair<std::string,cv::Rect>> keys;
        fdnn::fcv::processer::ocr::DetResult result;

        auto toString(){
            auto to_json=[&](nlohmann::json& j, const cv::Rect& rect){
                j = nlohmann::json{{"x", rect.x}, {"y", rect.y}, {"width", rect.width}, {"height", rect.height}};
            };
            nlohmann::json json;
            json["keys"] = nlohmann::json::array();
            for (const auto& [fst, snd] : keys) {
                nlohmann::json item;
                item["text"] = fst;
                to_json(item["rect"], snd);
                json["keys"].push_back(item);
            }
            return json.dump(2);
        }
    };
}
#endif //FASTSCREEN_FAA_RESULT_H