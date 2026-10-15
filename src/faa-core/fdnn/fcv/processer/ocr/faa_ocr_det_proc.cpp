//
// Created by HP on 2026/7/13.
//
#include "faa_core/fdnn/fcv/processer/ocr/faa_ocr_det_proc.h"
using faa::fdnn::fcv::processer::ocr::FaaOcrDetProc;
using faa::fdnn::fcv::processer::ocr::FaaDetPreParam;
using faa::fdnn::rt::code::FaaDnnRtCode;
void FaaOcrDetProc::PreProcess(const cv::Mat& img, cv::Mat& blob,const
    FaaDetPreParam& param, FaaDnnRtCode& code) {
    // std::cout<<param.height<<" "<<param.width<<std::endl;
    if (img.empty()){
        code=FaaDnnRtCode::Failed;
        return;
    }
    cv::dnn::blobFromImage(
        img.clone(),
        blob,param.scalefactor,
           cv::Size(param.width,param.height),
        cv::Scalar(0,255,0),
        false,
        false
    );
    // std::cout<<"blob shape "<<blob.rows<<" "<<blob.cols<<" "<<blob.channels()<<std::endl<<std::endl;
    code=FaaDnnRtCode::Success;

}

void FaaOcrDetProc::PostProcess(float* output, cv::Mat orgImg,
    const FaaDetPostParam& param,FaaDnnRtCode& code,DetResult& result) {

    if (output==nullptr || orgImg.empty()){
        code=FaaDnnRtCode::Failed;
        return;
    }
    cv::Mat probMap(param.height, param.width, CV_32F);
    cv::Mat(param.height, param.width, CV_32F, output).copyTo(probMap);
    double p_min, p_max;
    cv::minMaxLoc(probMap, &p_min, &p_max);
    if (p_max > p_min) {
        probMap = (probMap - p_min) / (p_max - p_min);
    }

    cv::Mat bin;
    cv::threshold(probMap, bin, param.threshold, 1.0, cv::THRESH_BINARY);
    bin.convertTo(bin, CV_8U, 255);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(bin, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    const double min_area = std::max(5.0, 30.0 *
        orgImg.cols * orgImg.rows / (param.width * param.height)
    );
    std::vector<cv::RotatedRect> textBoxes;

    for (const auto& c : contours) {
        double area = cv::contourArea(c);
        if (area < min_area) continue;
        cv::RotatedRect r = cv::minAreaRect(c);
        // unclip 膨胀：放大宽高，保持中心和角度不变
        r.size.width  *= param.unclip_ratio;
        r.size.height *= param.unclip_ratio;
        textBoxes.push_back(r);
    }
    float sx = static_cast<float>(orgImg.cols) / static_cast<float>(param.width);
    float sy = static_cast<float>(orgImg.rows) / static_cast<float>(param.height);
    for (const auto& rect : textBoxes) {
        // 1. 映射到原图坐标
        cv::Point2f center(rect.center.x * sx, rect.center.y * sy);
        cv::Size2f size(rect.size.width * sx, rect.size.height * sy);
        cv::RotatedRect scaled_rect(center, size, rect.angle);

        // 2. 取轴对齐包围盒 + 留白，直接从原图裁剪
        cv::Rect bbox = scaled_rect.boundingRect();
        bbox.x      = std::max(0, bbox.x - param.PAD);
        bbox.y      = std::max(0, bbox.y - param.PAD);
        bbox.width  = std::min(orgImg.cols - bbox.x, bbox.width + 2 * param.PAD);
        bbox.height = std::min(orgImg.rows - bbox.y, bbox.height + 2 * param.PAD);
        if (bbox.width <= 0 || bbox.height <= 0) continue;


        result.rects.push_back(bbox);
    }
    code=FaaDnnRtCode::Success;
}
