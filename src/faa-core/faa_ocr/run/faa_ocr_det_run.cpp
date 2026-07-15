//
// Created by HP on 2026/7/4.
//
#include <iostream>
#include <vector>
#include <gdlz/trt/core/cuda/op/trt_cuda_memory.h>
#include <gdlz/trt/cv/ocr/trt_cv_ocr_framework.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>
#include <gdlz/trt/cv/ocr/trt_cv_ocr_framework.h>
using namespace gdlz::trt::icv::ocr;
using namespace gdlz::trt::core;

auto post_process(float* arr,int rows,int cols,ProImg& proImg, std::vector<cv::RotatedRect>& textBoxes) {
    auto host_float=arr;
    cv::Mat probMap(rows, cols, CV_32F);
    cv::Mat(rows, rows, CV_32F, host_float).copyTo(probMap);

    double p_min, p_max;
    cv::minMaxLoc(probMap, &p_min, &p_max);
    std::cout << "probMap range: [" << p_min << ", " << p_max << "]" << std::endl;

    if (p_max > p_min) {
        probMap = (probMap - p_min) / (p_max - p_min);
    }
    cv::Mat bin;
    cv::threshold(probMap, bin, 0.01, 1.0, cv::THRESH_BINARY);
    bin.convertTo(bin, CV_8U, 255);

    const float unclip_ratio = 2.0f; // 提取轮廓
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(bin, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    const double min_area = std::max(5.0, 30.0 *
        proImg.org_mat->cols * proImg.org_mat->rows / (640.0 * 640.0)
    );

    for (const auto& c : contours) {
        double area = cv::contourArea(c);
        if (area < min_area) continue;
        cv::RotatedRect r = cv::minAreaRect(c);
        // unclip 膨胀：放大宽高，保持中心和角度不变
        r.size.width  *= unclip_ratio;
        r.size.height *= unclip_ratio;
        textBoxes.push_back(r);
    }

    std::cout << "found " << textBoxes.size() << " boxes (min_area=" << min_area << ")" << std::endl;

}

auto run_det(const std::string& config_path,const std::string& img_path){
    std::cout<<"config path: "<<config_path<<std::endl;
    std::cout<<"img path: "<<img_path<<std::endl;

    cv::Mat img = cv::imread(img_path);
    if (img.empty()){
        std::cerr<<"cannot read image "<<img_path<<std::endl;
        return;
    }
    auto framework = TensorRTCvOCRFramework(config_path.c_str());
    auto processer = new TensorRTCvOCRProcesserDet();
    auto ctx=TensorRTCvOCRCtx();
    auto output=cuda::TensorRTCudaData();
    auto proImg=ProImg();
    framework.CreateEngine();
    framework.GetEngineInfo();
    framework.CreateCtx(ctx);

    std::vector<int64_t> x_layer = {1,3,640,640};
    TensorRTCvOCRFramework::RegisterInputLager(ctx,"x",x_layer.data(),static_cast<int64_t>(x_layer.size()));
    TensorRTCvOCRFramework::RegisterOutputLager(ctx,"fetch_name_0",gdlz::trt::core::cuda::DataType::FLOAT32);
    auto img_clone=img.clone();
    TensorRTCvOCRFramework::MatToProImg(*processer,proImg,img_clone);
    TensorRTCvOCRFramework::PreProcess(*processer,proImg,{
        .img_width = 640,
        .img_height = 640,
        .scalefactor = 1/255.0
    });
    TensorRTCvOCRFramework::PostImg("x",ctx,*proImg.blob_mat,cuda::DataType::FLOAT32);
    for (int i=0;i<20;i++){
        auto start = std::chrono::high_resolution_clock::now();
        cuda::TensorRTCudaMemory::Sync(ctx);
        TensorRTCvOCRFramework::Forward(ctx);
        cuda::TensorRTCudaMemory::Sync(ctx);
        auto end = std::chrono::high_resolution_clock::now();  // 使用 double 类型存储毫秒，可以保留小数精度（例如 23.45ms）
        std::chrono::duration<double, std::milli> forward_time = end - start;
        std::cout << "forward time: " << forward_time.count() << " ms" << std::endl;
    }





    TensorRTCvOCRFramework::GetOutput("fetch_name_0",ctx,output,cuda::DataType::FLOAT32);


    auto host_float=static_cast<float*>(output.data);
    auto boxs=RectBoxs();
    TensorRTCvOCRFramework::PostProcess(*processer,host_float,proImg,PostParams{
        .img_width = 640,
        .img_height = 640,
        .PAD = 5,
        .unclip_ratio =2,
        .thresh = 0.001
    },boxs);

    int count=0;
    for (auto it: *(boxs.rect_mat)){

        auto split= (img)(it).clone();;

        cv::resize(split,split,cv::Size(split.cols*3,split.rows*3),cv::INTER_LINEAR);
        std::string str="./image/target/cropped_text_"+std::to_string(count++)+".png";

        cv::imwrite(str,split);
        std::cout<<"save: "<<str<<std::endl;
    }


    cv::imwrite("detection_result_v2.png", *proImg.org_mat);

}
int main(int argc, char* argv[]) {
    std::string config_path;
    std::string image_path;
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++){
        std::string arg = argv[i];
        args.push_back(arg);
    }
    int count=0;
    for (const auto& str:args){
        if (str == "-config"){
            if (count+1 > args.size()){
                std::cout<<"input -help get the help"<<std::endl;
                return -1;
            }
            config_path = args[count+1];
        }
        if (str=="-img"){
            if (count+1 > args.size()){
                std::cout<<"input -help get the help"<<std::endl;
                return -1;
            }
            image_path = args[count+1];
        }
        count++;
    }

    run_det(config_path,image_path);
    return 0;
}
