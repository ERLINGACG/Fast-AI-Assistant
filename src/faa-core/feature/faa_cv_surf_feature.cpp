//
// Created by HP on 2026/10/16.
//
#include "faa_core/feature/faa_cv_surf_feature.h"
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d/cuda.hpp>
using faa::core::fcv::feature::FaaCVSurfFeature;

FaaCVSurfFeature::FaaCVSurfFeature() : logger(nullptr) {

}



void FaaCVSurfFeature::setLogger(info::log::Ilog* logger) {
    this->logger = logger;
}

void FaaCVSurfFeature::runSurfToGPU (
    const cv::Mat& image,
    const FaaSurfParam& param,
    resources::FaaCvResource& resource,
    FaaSurfResult& result

    ) const {




    cv::Mat gray_image;
    if (image.channels() == 3) {
        cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);
    } else if (image.type() != CV_8UC1) {
        image.convertTo(gray_image, CV_8UC1);
    } else {
        gray_image = image;
    }
    auto gpu_Image = cv::cuda::GpuMat(gray_image);

    cv::Mat temp;
    bool found = resource.query(
        param.global_name,
        param.name,
        temp
        );

    if (!found) {
        if (this->logger) this->logger->error("FaaCVSurfFeature::runSurfToGPU: temp.image is empty");
        return;
    }
    if (temp.channels() == 3) {
        cv::cvtColor(temp, temp, cv::COLOR_BGR2GRAY);
    }
    // 如果 temp 是其他类型（如 CV_32F），也需要转换
    if (temp.type() != CV_8UC1) {
        temp.convertTo(temp, CV_8UC1);
    }
    cv::Mat tmpl_resized;
    constexpr int kMinSurfSize = 80;
    if (temp.cols < kMinSurfSize || temp.rows < kMinSurfSize) {
        double scale = std::max(kMinSurfSize / temp.cols, kMinSurfSize / temp.rows);
        cv::resize(temp, tmpl_resized, cv::Size(), scale, scale, cv::INTER_LINEAR);
        if (this->logger) this->logger->warn(
            "Template too small ({}x{}), resized to {}x{}",
            temp.cols, temp.rows, tmpl_resized.cols, tmpl_resized.rows);
        if (this->logger) this->logger->debug("is to resize");
    } else {
        tmpl_resized = temp;
    }
    auto gpu_temp = cv::cuda::GpuMat(tmpl_resized);

    cv::cuda::SURF_CUDA surf(
        param.hessianThreshold,
        param.nOctaves,
        param.nOctaveLayers,
        param.extended,
        param.keypointsRatio,
        param.upright
   );

    // 1. 提取模板特征点
    cv::cuda::GpuMat gpu_tmpl_kpts, gpu_tmpl_desc;
    surf(gpu_temp, cv::cuda::GpuMat(), gpu_tmpl_kpts, gpu_tmpl_desc);

    // 2. 提取模板特征点描述符
    std::vector<cv::KeyPoint> tmpl_kpts;
    std::vector<float> tmpl_desc_vec;
    surf.downloadKeypoints(gpu_tmpl_kpts, tmpl_kpts);
    surf.downloadDescriptors(gpu_tmpl_desc, tmpl_desc_vec);

    if (tmpl_kpts.size() < 4) {
        if (this->logger) this->logger->warn("FaaCVSurfFeature::runSurfToGPU: tmpl_kpts.size() < 4");
        return;
    }

    // 3. 提取场景特征点
    cv::cuda::GpuMat gpu_scene_kpts, gpu_scene_desc;
    surf(gpu_Image, cv::cuda::GpuMat(), gpu_scene_kpts, gpu_scene_desc);

    std::vector<cv::KeyPoint> scene_kpts;
    std::vector<float> scene_desc_vec;
    surf.downloadKeypoints(gpu_scene_kpts, scene_kpts);
    surf.downloadDescriptors(gpu_scene_desc, scene_desc_vec);

    if (scene_kpts.size() < 4) {
        if (this->logger) this->logger->warn("FaaCVSurfFeature::runSurfToGPU: scene_kpts.size() < 4");
        return;
    }

    int dim = param.extended ? 128 : 64;
    int tmpl_n  = static_cast<int>(tmpl_desc_vec.size())  / dim;
    int scene_n = static_cast<int>(scene_desc_vec.size()) / dim;

    cv::Mat tmpl_desc (tmpl_n,  dim, CV_32F, tmpl_desc_vec.data());
    cv::Mat scene_desc(scene_n, dim, CV_32F, scene_desc_vec.data());


    // 5. KNN 匹配 + 比率测试
    cv::BFMatcher matcher(cv::NORM_L2);
    std::vector<std::vector<cv::DMatch>> knn_matches;
    matcher.knnMatch(tmpl_desc, scene_desc, knn_matches, 2);

    std::vector<cv::DMatch> good_matches;
    good_matches.reserve(knn_matches.size());
    for (auto& m : knn_matches) {
        if (m.size() == 2 && m[0].distance < 0.75f * m[1].distance) {
            good_matches.push_back(m[0]);
        }
    }


    if (good_matches.size() < 4) {
        if (this->logger) this->logger->warn(
            "FaaCVSurfFeature::runSurfToGPU: good_matches.size() = {} < 4",
            good_matches.size());
        return;
    }

    // 6. 取点对，算单应性
    std::vector<cv::Point2f> tmpl_pts, scene_pts;
    tmpl_pts.reserve(good_matches.size());
    scene_pts.reserve(good_matches.size());
    for (auto& m : good_matches) {
        tmpl_pts.push_back(tmpl_kpts[m.queryIdx].pt);
        scene_pts.push_back(scene_kpts[m.trainIdx].pt);
    }

    std::vector<uchar> inliers_mask;
    cv::Mat H = cv::findHomography(tmpl_pts, scene_pts, cv::RANSAC, 3.0, inliers_mask);

    if (H.empty()) {
        if (this->logger) this->logger->warn(
            "FaaCVSurfFeature::runSurfToGPU: findHomography failed");
        return;
    }

    int inlier_count = cv::countNonZero(inliers_mask);
    if (inlier_count < 10) {
        if (this->logger) this->logger->warn(
            "FaaCVSurfFeature::runSurfToGPU: inlier_count = {} < 10", inlier_count);
        return;
    }

    // 7. 用内点重新精算一次 H（可选，但更稳）
    std::vector<cv::Point2f> in_tmpl, in_scene;
    in_tmpl.reserve(inlier_count);
    in_scene.reserve(inlier_count);
    for (size_t i = 0; i < inliers_mask.size(); ++i) {
        if (inliers_mask[i]) {
            in_tmpl.push_back(tmpl_pts[i]);
            in_scene.push_back(scene_pts[i]);
        }
    }
    cv::Mat H_refined = cv::findHomography(in_tmpl, in_scene, 0);

    // 8. 模板四角投影到场景
    std::vector<cv::Point2f> tmpl_corners = {
        {0.f, 0.f},
        {static_cast<float>(temp.cols), 0.f},
        {static_cast<float>(temp.cols), static_cast<float>(temp.rows)},
        {0.f, static_cast<float>(temp.rows)}
    };

    std::vector<cv::Point2f> scene_corners;
    cv::perspectiveTransform(tmpl_corners, scene_corners, H_refined);

    // 9. 算轴对齐包围盒
    float min_x = scene_corners[0].x, max_x = scene_corners[0].x;
    float min_y = scene_corners[0].y, max_y = scene_corners[0].y;
    for (auto& p : scene_corners) {
        min_x = std::min(min_x, p.x);
        max_x = std::max(max_x, p.x);
        min_y = std::min(min_y, p.y);
        max_y = std::max(max_y, p.y);
    }

    // 10. 裁剪到图像边界内
    min_x = std::max(0.f, min_x);
    min_y = std::max(0.f, min_y);
    max_x = std::min(static_cast<float>(image.cols), max_x);
    max_y = std::min(static_cast<float>(image.rows), max_y);

    if (max_x <= min_x || max_y <= min_y) {
        if (this->logger) this->logger->warn(
            "FaaCVSurfFeature::runSurfToGPU: box out of image range");
        return;
    }

    // 11. 填结果
    FaaSurfResult::Item item{};
    item.x = static_cast<int32_t>(min_x);
    item.y = static_cast<int32_t>(min_y);
    item.w = static_cast<int32_t>(max_x - min_x);
    item.h = static_cast<int32_t>(max_y - min_y);

    float inlier_ratio = static_cast<float>(inlier_count) / good_matches.size();
    item.score = static_cast<float>(inlier_count) * inlier_ratio;

    result.items.push_back(item);

    // 12. 日志
    if (this->logger) {
        this->logger->info(
            "FaaCVSurfFeature::runSurfToGPU: x={} y={} w={} h={} score={} "
            "good={} inliers={}",
            item.x, item.y, item.w, item.h, item.score,
            good_matches.size(), inlier_count);
    }




}
