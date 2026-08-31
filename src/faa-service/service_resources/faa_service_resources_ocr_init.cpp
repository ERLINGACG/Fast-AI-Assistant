//
// Created by HP on 2026/8/6.
//
#include "faa_service/resources/faa_service_resources_ocr.h"
#include  <Windows.h>
using faa::service::resources::ocr::FaaOcrRuntime;


void FaaOcrRuntime::detInit(nlohmann::json& config) const{


   for (auto& it : config){
      if (it.contains("det")) {
         try {
            auto det_meta=it["det"];
            det_dnn->initMeta(det_meta);
            det_dnn->initRt  (det_meta["path"].get<std::string>());
            for (auto& layer:det_meta["layers"].items()) {
                if (layer.value().contains("input")){
                  det_dnn->input(
                     det_dnn->entrance_layer,
                     layer.value()["input"]["dims"].get<std::vector<int64_t>>()
                  );
                  det_dnn->output(det_dnn->exit_layer,det_dnn->exit_type);
                }
            }
            break;
         } catch (const std::exception& e) {
            std::cerr << "C++ exception caught: " << e.what()<< std::endl;
         }
      }
   }
}

void FaaOcrRuntime::recInit(nlohmann::json& config) const {
   for (auto& it : config){
      if (it.contains("rec")) {
         try {
            auto rec_meta=it["rec"];
            rec_dnn->initMeta(rec_meta);
            rec_dnn->initRt  (rec_meta["path"].get<std::string>());
            rec_dnn->proc=std::make_unique<FaaOcrRecProc>(rec_meta["vocab_path"].get<std::string>());
            for (auto& layer:rec_meta["layers"].items()) {
               if (layer.value().contains("input")){

                  rec_dnn->input(
                     rec_dnn->entrance_layer,
                     layer.value()["input"]["dims"].get<std::vector<int64_t>>()
                  );
                  rec_dnn->output(rec_dnn->exit_layer,rec_dnn->exit_type);
               }
            }
            break;
         } catch (const std::exception& e) {
            std::cerr << "C++ exception caught: " << e.what()<< std::endl;
         }
      }
   }
};
