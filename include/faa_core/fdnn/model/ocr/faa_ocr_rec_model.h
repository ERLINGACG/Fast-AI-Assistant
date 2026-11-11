//
// Created by HP on 2026/11/5.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_OCR_REC__MODEL_H
#define FASTAIASSISTANT_ROOT_FAA_OCR_REC__MODEL_H
#include <fstream>

#include "faa_core/fdnn/general/blob/faa_cv_set_blob.h"
#include "faa_core/fdnn/general/ocr/faa_cv_ocr_det_param.h"
#include "faa_core/fdnn/model/base/faa_dnn_model_base.h"
#include "faa_core/fdnn/model/base/faa_dnn_model_layer.h"
#include "faa_core/faa_export.h"
#include "faa_core/fdnn/general/ocr/faa_cv_ocr_rec_param.h"

namespace faa::core::fdnn::model::ocr {




    class FAA_CPP_EXPORT FaaOcrRecModel : public base::FaaDnnModelBase {

        general::blob::FaaMean mean;
        layer::FaaDnnModelInputLayer  x {};
        layer::FaaDnnModelOutputLayer fetch_name_0 {};

        int img_h {};
        int img_w {};
        double scalefactor{};

        std::ifstream vocal_file;
        std::vector<std::string> vocab;

    public:
        void setConfig(const nlohmann::json& config);
        void loadVocab(const std::string& vocab_path);
        void loadVocab();
        void run  (
            frame::FaaMat& org,
            general::ocr::FaaRecParam& param,
            general::ocr::FaaDetResults& det_results,
            general::ocr::FaaRecResults& result
        );

        void run0   (
            frame::FaaMat& org,
            general::ocr::FaaRecParam& param,
            general::ocr::FaaDetResults& det_results,
            general::ocr::FaaRecResults& result
        );

    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_OCR_REC__MODEL_H
