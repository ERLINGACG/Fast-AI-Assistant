//
// Created by HP on 2026/9/3.
//

#ifndef FASTSCREEN_FAA_CLOUD_OCR_H
#define FASTSCREEN_FAA_CLOUD_OCR_H

#include "faa_core/faa_export.h"
#include "faa_core/fdnn/fcv/processer/ocr/faa_ocr_det_proc.h"
#include "faa_core/fdnn/runtime/faa_dnn_executor.h"

namespace faa::cloud::common::ocr{

    inline  std::map<std::string,gdlz::trt::core::cuda::DataType> types_mapping{
                  {"fp32",gdlz::trt::core::cuda::DataType::FLOAT32},
                  {"fp16",gdlz::trt::core::cuda::DataType::FLOAT16},
                  {"int8",gdlz::trt::core::cuda::DataType::INT8},
    };


    struct FaaCloudOcrAck{
        nlohmann::json data;
        fdnn::fcv::processer::ocr::DetResult det_result;
        std::vector<std::pair<std::string,cv::Rect>>   rec_result;

        auto detail(){
            data=nlohmann::json::object();
            auto arr=nlohmann::json::array();
            for(auto& [fst, snd]:rec_result) {

                    arr.push_back(
                        {
                            {"x",snd.x},
                            {"y",snd.y},
                            {"w",snd.width},
                            {"h",snd.height},
                            {"text",fst},
                        }
                    );
            }
            data["result"]=arr;
        }



    };




    template <typename B,typename C,typename P>
    struct FaaCloudOcr{

         using exc=std::unique_ptr<fdnn::rt::FaaDnnExecutor<B,C>>;
         using proc=std::unique_ptr<P>;


         std::string entrance_type{};
         std::string entrance_layer{};
         std::string exit_layer{};
         std::string exit_type{};
         exc executor;
         proc process;
         C    ctx;

         nlohmann::json config;

         auto& setConfig(const nlohmann::json& config){

             this->config=config;
             this->entrance_type =config.value("entrance_type","fp32");
             this->entrance_layer=config.value("entrance_layer","");
             this->exit_type     =config.value("exit_type","fp32");
             this->exit_layer    =config.value("exit_layer","");

             return *this;
         }

         auto& setProcess(std::unique_ptr<P> process){
             this->process = std::move(process);
             return *this;
         }


         auto& setExecutor(std::unique_ptr<fdnn::rt::FaaDnnExecutor<B,C>> executor){
             this->executor = std::move(executor);
             return *this;
         }

    };
}
#endif //FASTSCREEN_FAA_CLOUD_OCR_H