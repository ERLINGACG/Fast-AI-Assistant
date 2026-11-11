//
// Created by HP on 2026/11/5.
//
#include "faa_core/fdnn/general/ocr/faa_cv_ocr_rec_pros.h"

#include <iostream>
#include <ostream>

#include "faa_core/frame/private/faa_mat_impi.h"

void  faa::core::fdnn::general::ocr::recPostProcess(
    frame::FaaMat& blob,
    frame::FaaMat& orgImg,
    const FaaRecParam& param,
    const std::vector<std::string>& vocab,
    std::string& text
) {
    text.clear();
    auto* output = blob.impl()->mat.ptr<float>(0);

    const int blank_id = 0;   // PP-OCR CTC blank
    int last_idx = blank_id;

    for (int t = 0; t < param.seq_len; ++t) {
        float* time_step_data = output + t * param.model_vocal_size;

        int   max_idx = 0;
        float max_val = -std::numeric_limits<float>::infinity();
        for (int i = 0; i < param.model_vocal_size; ++i) {
            if (time_step_data[i] > max_val) {
                max_val = time_step_data[i];
                max_idx = i;
            }
        }
        if (max_idx != blank_id && max_idx != last_idx) {
            if (max_idx-1<vocab.size()){
                text+=vocab[max_idx-1];
            }
        }
        last_idx = max_idx;
    }

}
