//
// Created by HP on 2026/11/4.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_OCR_DET_PARAM_H
#define FASTAIASSISTANT_ROOT_FAA_OCR_DET_PARAM_H
#include <vector>
namespace faa::core::fdnn::general::ocr {

    struct FaaDetParam {

        double threshold = 0.3;
        int    pad = 2;
        bool   save_bin = false;
    };


    struct FaaDetResults {

        struct Item {
            int32_t x;
            int32_t y;
            int32_t w;
            int32_t h;
            double score;
        } ;

        std::vector<Item> items;
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_OCR_DET_PARAM_H