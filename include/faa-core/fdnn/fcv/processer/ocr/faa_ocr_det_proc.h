//
// Created by HP on 2026/7/13.
//

#ifndef FASTSCREEN_FAA_OCR_DET_PROC_H
#define FASTSCREEN_FAA_OCR_DET_PROC_H
#include <opencv2/opencv.hpp>
#include "faa-core/faa_export.h"
#include "faa-core/fdnn/runtime/faa_dnn_rt_code.h"
#include <nlohmann/json.hpp>
namespace faa::fdnn::fcv::processer::ocr {
    struct FaaDetPreParam{
        int32_t width;
        int32_t height;
        double  scalefactor;

    };

    struct FaaDetPostParam{
        int32_t width;
        int32_t height;
        float   threshold;
        float   unclip_ratio;
        int32_t PAD;
    };

    struct DetResult{

        std::vector<cv::Rect> rects;
        nlohmann::json data;
        auto toString() -> std::string{
             for (const auto& rect : rects){
                data["rects"].push_back( {std::pair<std::string,float>("x",rect.x),
                                          std::pair<std::string,float>("y",rect.y),
                                          std::pair<std::string,float>("width",rect.width),
                                          std::pair<std::string,float>("height",rect.height)});
             }
            return "";

        };
    };

    class FAA_CPP_EXPORT FaaOcrDetProc{
        public:
            FaaOcrDetProc() =default;
            ~FaaOcrDetProc()=default;
            void PreProcess  (const cv::Mat& img,cv::Mat& blob,
                              const FaaDetPreParam& param, rt::code::FaaDnnRtCode& code);
            void PostProcess (float* output,cv::Mat orgImg,
                              const FaaDetPostParam& param, rt::code::FaaDnnRtCode& code,DetResult& result);
    };
}
#endif //FASTSCREEN_FAA_OCR_DET_PROC_H