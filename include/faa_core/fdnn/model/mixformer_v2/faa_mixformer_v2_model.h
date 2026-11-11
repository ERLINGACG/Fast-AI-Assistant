//
// Created by HP on 2026/11/6.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_MIXFORMER_V2_MODEL_H
#define FASTAIASSISTANT_ROOT_FAA_MIXFORMER_V2_MODEL_H
#include "faa_core/faa_export.h"
#include "faa_core/fdnn/general/blob/faa_cv_set_blob.h"
#include "faa_core/fdnn/model/base/faa_dnn_model_base.h"
#include "faa_core/fdnn/model/base/faa_dnn_model_layer.h"

namespace faa::core::fdnn::model::mixformer_v2{


    struct FaaMixformerRunParam {

        int search_x = 0;          // 搜索起点 X
        int search_y = 0;          // 搜索起点 Y
        int search_w = 0;          // 搜索区域宽度 (0 表示全宽)
        int search_h = 0;          // 搜索区域高度 (0 表示全高)

        // 滑窗步长。越小越准（不漏目标），但耗时成倍增加；越大越快，但可能漏检。
        // 推荐值：112（即窗口大小 224 的一半，50% 重叠）。
        int step = 112;


        // ================= 2. 后处理过滤 (NMS & 阈值) =================
        // 置信度阈值。高于此值的框才会被保留。
        // 建议值：0.90 ~ 0.95。MixFormer 找真目标通常都很自信（接近 0.99）。
        float score_threshold = 0.90f;

        // NMS 重叠度阈值。两个框重叠面积超过这个比例，就删掉分低的那个。
        // 0.45 是业界标准值。
        float nms_threshold = 0.45f;

        // 最大保留目标数。如果业务上只找一个（比如右上角的 X），填 1 即可。
        int max_targets = 1;


        // ================= 3. 调试选项 =================
        // 是否自动画图并保存到本地（Debug 用）
        bool enable_debug_draw = false;
        // 保存路径
        std::string debug_save_path = "debug_result.png";
    };

    class FAA_CPP_EXPORT FaaMixformerV2Model : public base::FaaDnnModelBase {

        layer::FaaDnnModelInputLayer  img_t;
        layer::FaaDnnModelInputLayer  img_ot;
        layer::FaaDnnModelInputLayer  img_search;
        layer::FaaDnnModelOutputLayer pred_boxes;
        layer::FaaDnnModelOutputLayer pred_scores;

        general::blob::FaaMean mean;
        double scalefactor{};

        public:
            void setConfig(const nlohmann::json& config);
            void run (frame::FaaMat& org,frame::FaaMat& temp,const FaaMixformerRunParam& param);
            void run0(frame::FaaMat& org,frame::FaaMat& temp,const FaaMixformerRunParam& param);
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_MIXFORMER_V2_MODEL_H