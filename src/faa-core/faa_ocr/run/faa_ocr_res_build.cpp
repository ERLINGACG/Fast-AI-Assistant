//
// Created by HP on 2026/7/3.
//

#include <yaml-cpp/yaml.h>
#include <iostream>
#include <windows.h>
#undef ERROR
#include <gdlz/trt/core/trt_core_framework.h>
#include <gdlz/trt/cv/ocr/trt_cv_ocr_framework.h>
#include <dylog/logger.h>   // 现在 ERROR 宏已被清除
#include <gdlz/trt/core/cuda/op/trt_cuda_memory.h>

auto load_test(const std::vector<int>& non_zero_indices){
    try{
        YAML::Node config = YAML::LoadFile("./vos/vos.yaml");
        if (config["PostProcess"] && config["PostProcess"].IsMap()) {
            auto dict_node = config["PostProcess"]["character_dict"];

            // 检查节点存在且是数组类型
            if (dict_node && dict_node.IsSequence())
            {
                std::cout << "✅ 找到 character_dict！" << std::endl;
                std::cout << "   字典大小: " << dict_node.size() << std::endl;
                std::cout << "索引->字符映射：" << std::endl;
                for (int idx : non_zero_indices) {
                    std::string ch = dict_node[idx-1].as<std::string>();
                    std::cout << idx << " -> " << ch << std::endl;
                }

                std::cout << std::endl;
            } else {
                std::cout << "⚠️ character_dict 节点不存在或不是列表" << std::endl;
            }
        } else {
            std::cout << "⚠️ PostProcess 节点不存在或不是 Map" << std::endl;
        }

    }catch(YAML::Exception& e){
        std::cerr << "Error loading YAML file: " << e.what() << std::endl;
    }
}

auto build_faa_ocr(const std::string& config_path) {
    using gdlz::trt::core::TensorRTCoreFramework;
    using gdlz::trt::core::TensorRTCoreEngine;
    using gdlz::trt::core::TensorBuilderEngineInfo;
    using gdlz::trt::core::TensorRtBuilderShapeInfo;

    auto  engine=TensorRTCoreEngine();
    auto  engine_info=TensorBuilderEngineInfo();

    std::vector<int64_t> x_layer={1,3,48,320};
    auto  shap_x=TensorRtBuilderShapeInfo();
    auto  shap_fetch=TensorRtBuilderShapeInfo();

    auto  input_name=std::string("x");
    // auto  fetch_name=std::string("fetch_name_0");


    shap_x.name=const_cast<char*>(input_name.c_str());
    shap_x.K_min_shape=x_layer.data();
    shap_x.K_min_dims =static_cast<int64_t>(x_layer.size());
    shap_x.K_max_shape=x_layer.data();
    shap_x.K_max_dims =static_cast<int64_t>(x_layer.size());
    shap_x.K_opt_shape=x_layer.data();
    shap_x.K_opt_dims =static_cast<int64_t>(x_layer.size());


    TensorRTCoreFramework::InitShapeInfo(engine_info);
    TensorRTCoreFramework::RegisterShapeInfo(
        shap_x,
        engine_info
    );
    TensorRTCoreFramework::BuildEngine(engine,
        config_path.c_str(),
        engine_info
    );
}

auto run(const std::string& dir_path ,const std::string& config_path,const std::string& vos_path) {
    std::vector<cv::Mat> img_list;
    namespace fs = std::filesystem;
    fs::path dir = dir_path;
    for (const auto& entry : fs::directory_iterator(dir)) {
        // entry.path() 返回完整的路径对象
        std::cout << entry.path() << std::endl;
        std::string file_path = entry.path().string();
        img_list.push_back(cv::imread(file_path));
    }
    std::cout<<img_list.size()<<std::endl;

    using gdlz::trt::icv::ocr::TensorRTCvOCRFramework;
    auto ocr_framework = TensorRTCvOCRFramework(config_path.c_str());
    auto ocr_ctx = gdlz::trt::icv::ocr::TensorRTCvOCRCtx();

    ocr_framework.CreateEngine();
    ocr_framework.GetEngineInfo();
    ocr_framework.CreateCtx(ocr_ctx);

    std::vector<int64_t> x_layer={1,3,48,320};
    TensorRTCvOCRFramework::RegisterInputLager(ocr_ctx,"x",x_layer.data(),static_cast<int64_t>(x_layer.size()));
    TensorRTCvOCRFramework::RegisterOutputLager(ocr_ctx,"fetch_name_0",gdlz::trt::core::cuda::DataType::FLOAT32);
    std::vector<std::vector<int>> ids_list;
    for (auto img:img_list) {
        auto out_put = gdlz::trt::core::cuda::TensorRTCudaData();
        cv::resize(img, img, cv::Size(), 2.0, 2.0, cv::INTER_CUBIC);
        cv::Mat blob;
        cv::dnn::blobFromImage(img, blob, 1.0 / 255.0, cv::Size(320,48), cv::Scalar(), false, false);
        TensorRTCvOCRFramework::PostImg("x", ocr_ctx, blob, gdlz::trt::core::cuda::DataType::FLOAT32);
        TensorRTCvOCRFramework::Forward(ocr_ctx);
        TensorRTCvOCRFramework::GetOutput("fetch_name_0",ocr_ctx,out_put,gdlz::trt::core::cuda::DataType::FLOAT32);

        auto* output_data = static_cast<float*>(out_put.data);
        int seq_len = 40;
        int vocab_size = 18385;

        std::cout << "识别到的索引序列: ";
        std::vector<int> ids;
        for (int t = 0; t < seq_len; ++t) {
            // 找到当前时间步的最大值索引
            int max_idx = 0;
            float max_val = -1.0f;
            float* time_step_data = output_data + t * vocab_size;
            for (int i = 0; i < vocab_size; ++i) {
                if (time_step_data[i] > max_val) {
                    max_val = time_step_data[i];
                    max_idx = i;
                }
            }

            if (max_idx!=0 && max_idx <= 18383){
                std::cout << max_idx << " ";
                ids.push_back(max_idx);
            }
        }
        std::cout<<std::endl;
        ids_list.push_back(ids);
    }
    // for (auto it : ids_list){
    //     // load_test(it);
    // }
    //
    std::vector<std::string> str_list;
    YAML::Node config = YAML::LoadFile(vos_path);
    auto dict_node = config["PostProcess"]["character_dict"];
    for (const auto& it:ids_list){
        std::string it_info;
        for (auto i: it){
            it_info+=std::to_string(i)+ " ";
        }
        try {
            std::string str;
            for (int idx : it) {
                auto ch = dict_node[idx-1].as<std::string>();
                str+=ch;
            }
            str_list.push_back(str);
        }catch (std::exception& e){
            std::cerr<<e.what()<<" to : " <<it_info<<std::endl;
        }

    }
    std::cout<<str_list.size()<<std::endl;
    int count = 0;
    for (const auto& it:str_list){

        std::cout<<"count:"<<count<<" "<<it<<std::endl;
        count++;
    }


}

auto test_run(std::string config_path) {

    auto img=cv::imread("./cropped_text_0.jpg");
    cv::resize(img, img, cv::Size(320, 48));
    cv::Mat blob;
    cv::dnn::blobFromImage(img, blob, 1.0 / 255.0, cv::Size(), cv::Scalar(), false, false);

    // 计算总字节数
    size_t data_bytes = blob.total() * blob.elemSize();  // 对于 CV_32F，elemSize() = 4

    using gdlz::trt::icv::ocr::TensorRTCvOCRFramework;
    auto ocr_framework = TensorRTCvOCRFramework(config_path.c_str());
    auto ocr_ctx = gdlz::trt::icv::ocr::TensorRTCvOCRCtx();
    auto out_put = gdlz::trt::core::cuda::TensorRTCudaData();

    ocr_framework.CreateEngine();
    ocr_framework.GetEngineInfo();
    ocr_framework.CreateCtx(ocr_ctx);

    std::vector<int64_t> x_layer={1,3,48,320};
    TensorRTCvOCRFramework::RegisterInputLager(ocr_ctx,"x",x_layer.data(),static_cast<int64_t>(x_layer.size()));
    TensorRTCvOCRFramework::RegisterOutputLager(ocr_ctx,"fetch_name_0",gdlz::trt::core::cuda::DataType::FLOAT32);
    TensorRTCvOCRFramework::PostImg("x", ocr_ctx, blob.data, data_bytes, gdlz::trt::core::cuda::DataType::FLOAT32);

    auto start = std::chrono::high_resolution_clock::now();
    TensorRTCvOCRFramework::Forward(ocr_ctx);
    auto end = std::chrono::high_resolution_clock::now();
    gdlz::trt::core::cuda::TensorRTCudaMemory::Sync(ocr_ctx);
    std::cout << "Forward time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

    TensorRTCvOCRFramework::GetOutput("fetch_name_0",ocr_ctx,out_put,gdlz::trt::core::cuda::DataType::FLOAT32);
    std::cout << "out_put.shape: " << out_put.len << std::endl;
    float* output_data = static_cast<float*>(out_put.data);
    int seq_len = 40;
    int vocab_size = 18385;

    std::cout << "识别到的索引序列: ";
    std::vector<int> ids;
    for (int t = 0; t < seq_len; ++t) {
        // 找到当前时间步的最大值索引
        int max_idx = 0;
        float max_val = -1.0f;
        float* time_step_data = output_data + t * vocab_size;
        for (int i = 0; i < vocab_size; ++i) {
            if (time_step_data[i] > max_val) {
                max_val = time_step_data[i];
                max_idx = i;
            }
        }
        std::cout << max_idx << " ";
        if (max_idx!=0){
            ids.push_back(max_idx);
        }

    }
    std::cout << std::endl;
    load_test(ids);
    // YAML::Node config = YAML::LoadFile("./vos/vos.yaml");

}

auto test_run_2(const std::string& config_path) {
    auto img=cv::imread("./image/test11.png");

    using gdlz::trt::icv::ocr::TensorRTCvOCRFramework;

    auto ocr_framework = TensorRTCvOCRFramework(config_path.c_str());
    auto ocr_ctx = gdlz::trt::icv::ocr::TensorRTCvOCRCtx();
    auto out_put = gdlz::trt::core::cuda::TensorRTCudaData();
    auto procersser=new gdlz::trt::icv::ocr::TensorRTCvOCRProcesserReC();

    auto proMat=gdlz::trt::icv::ocr::ProImg();
    ocr_framework.CreateEngine();
    ocr_framework.GetEngineInfo();
    ocr_framework.CreateCtx(ocr_ctx);


    // TensorRTCvOCRFramework::SetProcess(ocr_ctx,"rec",new gdlz::trt::icv::ocr::TensorRTCvOCRProcesserReC());
    //
    TensorRTCvOCRFramework::MatToProImg(
        *procersser,
        proMat,
        img);
    TensorRTCvOCRFramework::PreProcess(*procersser,proMat, gdlz::trt::icv::ocr::PreParams{
         .img_width = 320,
        .img_height = 48,
        .scalefactor = 1/255.0,
    });
    // std::cout<<proMat.org_mat->total()<<std::endl;
    std::cout<<proMat.blob_mat->total()<<std::endl;
    std::vector<int64_t> x_layer={1,3,48,320};
    TensorRTCvOCRFramework::RegisterInputLager(ocr_ctx,"x",x_layer.data(),static_cast<int64_t>(x_layer.size()));
    TensorRTCvOCRFramework::RegisterOutputLager(ocr_ctx,"fetch_name_0",gdlz::trt::core::cuda::DataType::FLOAT32);
    TensorRTCvOCRFramework::PostImg("x", ocr_ctx, proMat.blob_mat->data, proMat.blob_mat->total(), gdlz::trt::core::cuda::DataType::FLOAT32);
    TensorRTCvOCRFramework::Forward(ocr_ctx);


}

int main(){
    SetConsoleOutputCP(CP_UTF8);
    run(".\\image\\target","./config/trt_ocr_rec.json","./vos/vos.yaml");
    // test_run("./config/trt_ocr_rec.json");
    // test_run_2("./config/trt_ocr_rec.json");
    // load_test({
    //     16167,16353,16155,1233,18383,3233,11,1646,893,207,1496,1156,439,1156,389 ,25 ,16337
    // });
    return 0;
}