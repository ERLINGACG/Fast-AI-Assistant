//
// Created by HP on 2026/7/10.
//

#include <iostream>
#include <gdlz/trt/cv/core/trt_cv_core.h>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>

int main(int argc, char* argv[]){
    using  gdlz::trt::core::cuda::DataType;
    using  gdlz::trt::icv::core::TensorRTCvCoreFramework;
    using  gdlz::trt::icv::core::TensorRTCvCtx;
    using  gdlz::trt::core::cuda::TensorRTCudaData;
    auto image=cv::imread("./image/Endfield_0.jpg");
    if (image.empty()){

        std::cerr<<"image is empty"<<std::endl;
        return -1;
    }
    cv::Mat blob;
    cv::dnn::blobFromImage(image.clone(),blob,1/255.0,cv::Size(640,640),false,false);

    auto framework = TensorRTCvCoreFramework("./config/trt_yolo_run.json");
    auto ctx   = TensorRTCvCtx();
    auto output=TensorRTCudaData();
    framework.CreateEngine();
    framework.GetEngineInfo();
    framework.CreateCtx(ctx);

    std::vector<int64_t> image_shape={1,3,640,640};
    framework.RegisterInputLayer (ctx,"images",image_shape.data(),image_shape.size());
    framework.RegisterOutputLayer(ctx,"output0",DataType::FLOAT32);
    framework.PostImg("images",ctx,blob.data,blob.total(),DataType::FLOAT32);
    framework.Forward(ctx);
    framework.GetOutput("output0",ctx,output,DataType::FLOAT32);

    float* data = static_cast<float*>(output.data);

    int numChannels = 9;      // 对应输出张量的第1维（9）
    int numAnchors = 8400;    // 对应输出张量的第2维（8400）

    float confThreshold = 0.5f;
    float nmsThreshold = 0.4f;

    // 原始图像尺寸
    int origW = image.cols;
    int origH = image.rows;
    int inputW = 640, inputH = 640;

    float x_factor = static_cast<float>(origW) / inputW;
    float y_factor = static_cast<float>(origH) / inputH;

    // 存储检测结果
    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;
    std::vector<int> classIds;

    for (int w = 0; w < numAnchors; ++w) {
        // 取最大类别分数（类别从索引4开始，共5个类别，即索引4~8）
        float maxScore = 0.0f;
        int classId = -1;
        for (int c = 0; c < 5; ++c) {
            float score = data[(4 + c) * numAnchors + w];
            if (score > maxScore) {
                maxScore = score;
                classId = c;
            }
        }
        if (maxScore < confThreshold) continue;

        // 读取坐标（通道0~3）
        float cx = data[0 * numAnchors + w];
        float cy = data[1 * numAnchors + w];
        float w_box = data[2 * numAnchors + w];
        float h_box = data[3 * numAnchors + w];

        // 将归一化坐标缩放到原始图像尺寸
        int x1 = static_cast<int>((cx - w_box / 2) * x_factor);
        int y1 = static_cast<int>((cy - h_box / 2) * y_factor);
        int x2 = static_cast<int>((cx + w_box / 2) * x_factor);
        int y2 = static_cast<int>((cy + h_box / 2) * y_factor);

        // 边界裁剪（防止越界）
        x1 = std::max(0, x1);
        y1 = std::max(0, y1);
        x2 = std::min(origW, x2);
        y2 = std::min(origH, y2);

        boxes.emplace_back(x1, y1, x2 - x1, y2 - y1);
        confidences.push_back(maxScore);
        classIds.push_back(classId);
    }

    // NMS
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

    // 绘制结果
    for (int idx : indices) {
        auto& box = boxes[idx];
        cv::rectangle(image, box, cv::Scalar(0, 255, 0), 2);
        std::string label = std::to_string(confidences[idx]).substr(0, 4);
        cv::putText(image, label,
                    cv::Point(box.x, box.y - 5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0));
    }
    cv::imshow("Detection", image);
    cv::waitKey(0);
    return 0;
}
