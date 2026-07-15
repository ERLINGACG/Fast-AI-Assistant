//
// Created by HP on 2026/7/9.
//
#include <iostream>
#include <vector>
#include <gdlz/trt/core/trt_core_data.h>
#include <gdlz/trt/core/trt_core_framework.h>
// #include <gdlz/trt/cv/core/trt_cv_core.h>

#include <gdlz/trt/cv/ocr/trt_cv_ocr_framework.h>
#include <nlohmann/json.hpp>
#include "info/ilog/ilog.h"
using info::log::Ilog;
using info::log::IlogInit;
using gdlz::trt::core::TensorRTCoreFramework;
using gdlz::trt::core::TensorRtBuilderShapeInfo;
using gdlz::trt::core::TensorBuilderEngineInfo;
using gdlz::trt::core::TensorRTCoreEngine;

using nlohmann::json;
int main(int argc, char* argv[]){
    auto log = Ilog([](){
        return IlogInit{};
    });
    std::vector<std::string> args(argv+1, argv + argc);
    std::string config;
    std::string layers_info;
    int count=0;
    for (const auto& arg : args){
        if (arg == "-config"){
            if (count+1>args.size()){
                std::cerr<<"config path is not specified"<<std::endl;
                return -1;
            }
            config=args[count+1];
        }
        if (arg == "-layers"){
            if (count+1>args.size()){
                std::cerr<<"config path is not specified"<<std::endl;
                return -1;
            }
            layers_info=args[count+1];
        }
        count++;
    }
    log.info("config is:{}",config);
    try {
        json layers_json = json::parse(layers_info);
        if (!layers_json.contains( "layers")){
            log.error("no layers info found");
            return -1;
        }
        auto  engine_info=TensorBuilderEngineInfo();
        auto  engine     =TensorRTCoreEngine();
        TensorRTCoreFramework::InitShapeInfo(engine_info);

        std::vector<std::string> name_storage;                 // 保存名字
        std::vector<std::vector<int64_t>> min_storage;         // 保存 min_shape
        std::vector<std::vector<int64_t>> max_storage;         // 保存 max_shape
        std::vector<std::vector<int64_t>> opt_storage;         // 保存 opt_shape
        for (auto& layer : layers_json["layers"]) {

            if (!layer.contains("name") ||
                !layer.contains("min_shape") ||
                !layer.contains("max_shape") ||
                !layer.contains("opt_shape")
                ){
                log.warn("layer info is not standard:{}",layer.dump());
                continue;
            }
            auto name = layer["name"].get<std::string>();
            auto min_shape = layer["min_shape"].get<std::vector<int64_t>>();
            auto max_shape = layer["max_shape"].get<std::vector<int64_t>>();
            auto opt_shape = layer["opt_shape"].get<std::vector<int64_t>>();
            std::string min_info="[";
            for (auto& dim : min_shape){
                min_info+=std::to_string(dim)+",";
            }
            min_info.pop_back();
            min_info+="]";

            std::string max_info="[";
            for (auto& dim : max_shape){
                max_info+=std::to_string(dim)+",";
            }
            max_info.pop_back();
            max_info+="]";

            std::string opt_info="[";
            for (auto& dim : opt_shape){
                opt_info+=std::to_string(dim)+",";
            }
            opt_info.pop_back();
            opt_info+="]";

            log.info("layer {} min max opt info is:{}",name,min_info,max_info,opt_info);
            name_storage.push_back(name);                     // 拷贝字符串
            min_storage.push_back(min_shape);                 // 拷贝 vector
            max_storage.push_back(max_shape);
            opt_storage.push_back(opt_shape);


        }
        if (name_storage.size() != min_storage.size() ||
            name_storage.size() != max_storage.size() ||
            name_storage.size() != opt_storage.size()){
            log.error("name min_shape max_shape opt_shape size is not equal");
            return -1;
        }
        auto sizes=name_storage.size();

        for (int i=0;i<sizes;i++){
            auto shape_info=TensorRtBuilderShapeInfo{
                const_cast<char*>(name_storage[i].c_str()),
                static_cast<int64_t>(min_storage[i].size()),
                min_storage[i].data(),
                static_cast<int64_t>(max_storage[i].size()),
                max_storage[i].data(),
                static_cast<int64_t>(opt_storage[i].size()),
                opt_storage[i].data(),
            };
            TensorRTCoreFramework::RegisterShapeInfo(
                shape_info,
                engine_info
            );
        }

        TensorRTCoreFramework::BuildEngine(engine,
        config.c_str(),
            engine_info
        );


    } catch (const json::parse_error& e) {
        std::cerr << "Error parsing layers JSON: " << e.what() << std::endl;
        return -1;
    }
    return 0;

}