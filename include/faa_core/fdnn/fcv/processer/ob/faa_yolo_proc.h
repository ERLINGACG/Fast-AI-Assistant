//
// Created by HP on 2026/7/13.
//

#ifndef FASTSCREEN_FAA_YOLO_PROC_H
#define FASTSCREEN_FAA_YOLO_PROC_H
#include "faa_core/fdnn/fcv/faa_dnn_cv_trt.h"
#include <nlohmann/json.hpp>
#include <opencv2/core/mat.hpp>

#include "faa_core/faa_export.h"

namespace faa::fdnn::fcv::processer::ob {

    struct FaaYoloParams{

        int  input_width =640;
        int  input_height=640;
        bool isDraw      =true;
        float conf       =0.5f;
        float iou        =0.5f;
        float maxScore   =0.1f;
    };


    struct FaaYoloResult {

        struct ObjectInfo {
            int32_t x;
            int32_t y;
            int32_t w;
            int32_t h;
            float conf;
            int class_id;
            std::string class_name;
        } ;

        std::vector<ObjectInfo> results;


        auto push(const ObjectInfo& obj){
            results.push_back(obj);
        }

        auto serialize(){
            nlohmann::json data;
            data["results"]=nlohmann::json::array();
            for (auto& obj:results){
                data["results"].push_back({
                    {"x",obj.x},
                    {"y",obj.y},
                    {"w",obj.w},
                    {"h",obj.h},
                    {"conf",obj.conf},
                    {"class_id",obj.class_id},
                    {"class_name",obj.class_name}
                });
            }
            return data;
        }

    };

    class FAA_CPP_EXPORT FaaYoloProc{

        nlohmann::json config{};

        std::vector<std::string> class_labs{};
        int32_t numAnchors = 8400;

        public:
            FaaYoloProc(const nlohmann::json& config);
            void PreProcess  (const cv::Mat& img,cv::Mat& blob,const FaaYoloParams& params);
            void PostProcess (const float* output, cv::Mat& orgImg,  FaaYoloParams& params, FaaYoloResult& result);
    };
}
#endif //FASTSCREEN_FAA_YOLO_PROC_H