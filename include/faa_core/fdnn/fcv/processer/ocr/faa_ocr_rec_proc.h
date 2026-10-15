//
// Created by HP on 2026/7/13.
//

#ifndef FASTSCREEN_FAA_OCR_REC_PROC_H
#define FASTSCREEN_FAA_OCR_REC_PROC_H
#include <fstream>

#include "faa_core/faa_export.h"
#include "faa_core/fdnn/runtime/faa_dnn_rt_code.h"
namespace faa::fdnn::fcv::processer::ocr {

    struct FaaResParam{
        int32_t width;
        int32_t height;
        double  scalefactor;

    };

    struct FaaPostParam{
        int32_t model_vocal_size;
        int32_t seq_len;
    };

    struct FaaResResult{
        std::vector<int> ids;
    };

    class FAA_CPP_EXPORT FaaOcrRecProc {
         std::ifstream vocal_file;
         std::vector<std::string> vocal;
         public:
            FaaOcrRecProc (const std::string& vocal_path);
            ~FaaOcrRecProc()=default;

            void PreProcess(cv::Mat& img,cv::Mat& blob,FaaResParam param, rt::code::FaaDnnRtCode& code);
            void PostProcess(float* output,FaaPostParam param,FaaResResult& result, rt::code::FaaDnnRtCode& code);
            auto& file(){
                return this->vocal_file;
            }
            auto& get_vocal(){
                return vocal;
            }
            // void PostProcess(float* output, rt::code::FaaDnnRtCode& code);

    };
}
#endif //FASTSCREEN_FAA_OCR_REC_PROC_H