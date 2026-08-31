//
// Created by HP on 2026/8/10.
//

#ifndef FASTSCREEN_FAA_SERVICE_OCR_OPERATION_H
#define FASTSCREEN_FAA_SERVICE_OCR_OPERATION_H
#include "faa_service/resources/faa_service_resources.h"

namespace faa::service::operation::ocr{

    struct  OcrAckResult{

        fdnn::fcv::processer::ocr::DetResult result;
        std::vector<std::pair<std::string,cv::Rect>> key_values;

        nlohmann::json data;



        auto toString() -> std::string {
            return this->toData().dump(2);
        };

       auto& toData(){
           data=nlohmann::json::array();
           for (const auto& [key,rect] : key_values){
               nlohmann::json item;
               item["str"] = key;
               item["rect"] = {
                   {"x", rect.x},
                   {"y", rect.y},
                   {"width", rect.width},
                   {"height", rect.height}
               };
               data.push_back(item);
           }
           return this->data;
       }
    };

    void get_to_ocr_det(nlohmann::json& params,resources::FaaServiceResources& resources,const std::string& global_name,OcrAckResult& result);

    template<typename F>
    void get_to_ocr_det(nlohmann::json& params,resources::FaaServiceResources& resources,const std::string& global_name,OcrAckResult& result,F&& callback){
        get_to_ocr_det(params,resources,global_name,result);
        callback();
    }


}
#endif //FASTSCREEN_FAA_SERVICE_OCR_OPERATION_H