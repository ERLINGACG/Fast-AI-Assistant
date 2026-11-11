//
// Created by HP on 2026/11/6.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CV_MIXFORMER_V2_PROS_H
#define FASTAIASSISTANT_ROOT_FAA_CV_MIXFORMER_V2_PROS_H
#include "faa_cv_mixformer_v2_param.h"
#include "faa_core/frame/faa_mat.h"

namespace faa::core::fdnn::general::mixformer_v2 {




    void mixFormerPresImg (
        const PerParam& param,
        const frame::FaaMat& org,
        std::vector<std::pair<frame::FaaMat, RecBox>>& imgs
    );

    void mixFormerPostprocess (
        frame::FaaMat& org,
        frame::FaaMat& roi_org,
        frame::FaaMat& pred_boxes_blob,
        frame::FaaMat& pred_scores_blob,
        const RecBox& cropBox
    );

    std::vector<MixformerDet> mixFormerNMS(
        const std::vector<MixformerDet>& dets,
        float score_thresh,
        float nms_thresh
    );

    void mixFormerDraw(
        frame::FaaMat& org,
        const std::vector<MixformerDet>& dets,
        const std::string& save_path = ""
    );

    void mixFormerPostprocess(
        const frame::FaaMat& org,
        frame::FaaMat& pred_boxes_blob,
        frame::FaaMat& pred_scores_blob,
        const RecBox& cropBox,
        std::vector<MixformerDet>& out_dets
    );
}
#endif //FASTAIASSISTANT_ROOT_FAA_CV_MIXFORMER_V2_PROS_H