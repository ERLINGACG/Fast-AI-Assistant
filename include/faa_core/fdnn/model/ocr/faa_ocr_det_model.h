//
// Created by HP on 2026/11/3.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_OCR_DET_MODEL_H
#define FASTAIASSISTANT_ROOT_FAA_OCR_DET_MODEL_H
#include "faa_core/fdnn/model/base/faa_dnn_model_base.h"
#include "faa_core/faa_export.h"
#include "faa_core/fdnn/general/blob/faa_cv_set_blob.h"
#include "faa_core/fdnn/general/ocr/faa_cv_ocr_det_param.h"
#include "faa_core/fdnn/model/base/faa_dnn_model_layer.h"

namespace faa::core::fdnn::model::ocr {

      struct FaaDetRunParam {

         general::ocr::FaaDetParam param{};
         bool isSave = false;

      };



      class FAA_CPP_EXPORT FaaOcrDetModel : public base::FaaDnnModelBase {

            general::blob::FaaMean mean;
            layer::FaaDnnModelInputLayer  x {};
            layer::FaaDnnModelOutputLayer fetch_name_0 {};

            int img_h {};
            int img_w {};
            double scalefactor{};

      public:

            void setConfig(const nlohmann::json& config);
            void run (frame::FaaMat& img, const FaaDetRunParam& param, general::ocr::FaaDetResults& results);
            void run0(frame::FaaMat& img, const FaaDetRunParam& param, general::ocr::FaaDetResults& results);

      };
}
#endif //FASTAIASSISTANT_ROOT_FAA_OCR_DET_MODEL_H