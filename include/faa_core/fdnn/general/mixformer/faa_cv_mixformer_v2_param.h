//
// Created by HP on 2026/11/6.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CV_MIXFORMER_V2_PARAM_H
#define FASTAIASSISTANT_ROOT_FAA_CV_MIXFORMER_V2_PARAM_H
namespace faa::core::fdnn::general::mixformer_v2 {
    struct PerParam {

        int copy_x = 0;
        int copy_y = 0;
        int copy_w = 0;
        int copy_h = 0;
        int step = 0;
    };

    struct RecBox {
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;
    };

    struct MixformerDet {
        RecBox box;
        float score = 0.f;
    };

}
#endif //FASTAIASSISTANT_ROOT_FAA_CV_MIXFORMER_V2_PARAM_H