//
// Created by HP on 2026/11/6.
//
#include "faa_core/fdnn/model/mixformer_v2/faa_mixformer_v2_model.h"

#include "faa_core/fdnn/backend/faa_backend_type_oper.h"
#include "faa_core/fdnn/general/mixformer/faa_cv_mixformer_v2_param.h"
#include "faa_core/fdnn/general/mixformer/faa_cv_mixformer_v2_pros.h"
using faa::core::fdnn::model::mixformer_v2::FaaMixformerV2Model;

void FaaMixformerV2Model::setConfig(const nlohmann::json& config)
{
    FaaDnnModelBase::setConfig(config);
    auto means      = config.value("mean",nlohmann::json::object());
    auto img_t      = config.value("temp_img_layer",nlohmann::json::object());
    auto img_ot     = config.value("online_img_layer",nlohmann::json::object());
    auto img_search = config.value("search_img_layer",nlohmann::json::object());
    auto pred_boxes = config.value("pred_boxes_layer",nlohmann::json::object());
    auto pred_scores= config.value("pred_scores_layer",nlohmann::json::object());

    this->mean.b        = means.value("b",0.0);
    this->mean.g        = means.value("g",0.0);
    this->mean.r        = means.value("r",0.0);
    this->scalefactor   = this->config.value("scale",1.0);

    layer::build_InputLayer_to_json(this->img_t,img_t);
    layer::build_InputLayer_to_json(this->img_ot,img_ot);
    layer::build_InputLayer_to_json(this->img_search,img_search);

    layer::build_OutputLayer_to_json(this->pred_boxes,pred_boxes);
    layer::build_OutputLayer_to_json(this->pred_scores,pred_scores);

}

void FaaMixformerV2Model::run(frame::FaaMat& org, frame::FaaMat& temp, const FaaMixformerRunParam& param) {
    backend::visit_to_type(
        this->type,
        [this,&org,&temp,&param]() {
             run0(org,temp,param);
        },
        [](){

        }
    );
}

void FaaMixformerV2Model::run0(frame::FaaMat& org, frame::FaaMat& temp, const FaaMixformerRunParam& param) {

    auto start = std::chrono::high_resolution_clock::now();

    using general::mixformer_v2::mixFormerPresImg;
    using general::mixformer_v2::mixFormerPostprocess;
    using general::mixformer_v2::RecBox;
    using general::mixformer_v2::MixformerDet;

    std::vector<std::pair<frame::FaaMat, RecBox>> images;



    frame::FaaMat blob_t;
    frame::FaaMat blob_ot;
    auto t_h = img_t.opt_shape[2];
    auto t_w = img_t.opt_shape[3];

    auto ot_h = img_ot.opt_shape[2];
    auto ot_w = img_ot.opt_shape[3];

    auto search_h = img_search.opt_shape[2];
    auto search_w = img_search.opt_shape[3];

    int sx = param.search_x;
    int sy = param.search_y;
    int sw = (param.search_w > 0) ? param.search_w : static_cast<int>(org.col());
    int sh = (param.search_h > 0) ? param.search_h : static_cast<int>(org.row());


    mixFormerPresImg(
        general::mixformer_v2::PerParam {
             .copy_x = sx,
             .copy_y = sy,
             .copy_w = sw,
             .copy_h = sh,
             .step   = param.step,
        },
        org,
        images
    );


    general::blob::setBlob(blob_t, temp,{
            .mean = this->mean,
            .h =  static_cast<int>(t_h),
            .w =  static_cast<int>(t_w),
            .scalefactor = this->scalefactor,
            .swapRB = false,
            .crop   = false,
       });

    general::blob::setBlob(blob_ot,temp,{
         .mean = this->mean,
         .h =  static_cast<int>(ot_h),
         .w =  static_cast<int>(ot_w),
         .scalefactor = this->scalefactor,
         .swapRB = false,
         .crop   = false,
    });
    getBackend0().setInputLayerShape(img_t.name,     img_t.opt_shape);
    getBackend0().setInputLayerShape(img_ot.name,    img_ot.opt_shape);
    getBackend0().setInputLayerShape(img_search.name,img_search.opt_shape);
    getBackend0().setOutputLayerShape(pred_boxes.name,pred_boxes.dtype);
    getBackend0().setOutputLayerShape(pred_scores.name,pred_scores.dtype);

    getBackend0().uploadBlob(img_t.name, blob_t );
    getBackend0().uploadBlob(img_ot.name,blob_ot);

    std::vector<MixformerDet> all_dets;
    if (ilogger) ilogger->debug("roi count: {}",images.size());
    for (auto& img : images) {

        // auto start = std::chrono::high_resolution_clock::now();
        frame::FaaMat blob_search;
        frame::FaaMat blob_pred_boxes;
        frame::FaaMat blob_pred_scores;

        general::blob::setBlob(blob_search, img.first,{
             .mean = this->mean,
             .h =  static_cast<int>(search_h),
             .w =  static_cast<int>(search_w),
             .scalefactor = this->scalefactor,
             .swapRB = false,
             .crop   = false,
        });

        getBackend0().uploadBlob(img_search.name,blob_search);
        getBackend0().forward();

        getBackend0().downloadBlob(pred_boxes.name, blob_pred_boxes ,pred_boxes.dtype);
        getBackend0().downloadBlob(pred_scores.name,blob_pred_scores,pred_scores.dtype);
        mixFormerPostprocess(
            org,
            blob_pred_boxes,
            blob_pred_scores,
            img.second,
            all_dets);
        // auto end = std::chrono::high_resolution_clock::now();
        // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        // if (ilogger) ilogger->debug("the run time: {}",duration.count());
    }


    std::vector<MixformerDet> final_dets = mixFormerNMS(
        all_dets,
        param.score_threshold,
        param.nms_threshold);





    // 统一画图并保存（只调用一次，不再在内层循环里疯狂写盘）
    if (param.max_targets > 0 && final_dets.size() > static_cast<size_t>(param.max_targets)) {
        final_dets.resize(param.max_targets);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    if (ilogger) ilogger->debug("all time: {}",duration.count());

    // 画图控制
    auto draw_start = std::chrono::high_resolution_clock::now();
    if (param.enable_debug_draw) {
        mixFormerDraw(org, final_dets, param.debug_save_path);
    }
    auto draw_end = std::chrono::high_resolution_clock::now();
    auto draw_duration = std::chrono::duration_cast<std::chrono::milliseconds>(draw_end - draw_start);
    if (ilogger) ilogger->debug("draw time: {}",draw_duration.count());



}
