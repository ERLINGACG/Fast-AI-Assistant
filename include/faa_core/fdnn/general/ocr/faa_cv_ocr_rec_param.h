//
// Created by HP on 2026/11/5.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CV_OCR_REC_PARAM_H
#define FASTAIASSISTANT_ROOT_FAA_CV_OCR_REC_PARAM_H
#include <string>
#include <vector>

namespace faa::core::fdnn::general::ocr {


    struct FaaRecParam {
        int seq_len;
        int model_vocal_size;
    };

    struct  FaaRecResults {

        struct recItem {
            std::string text;
            double score;
            int x, y, w, h;
        };
        std::vector<recItem> items;
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_CV_OCR_REC_PARAM_H