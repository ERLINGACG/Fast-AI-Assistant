//
// Created by HP on 2026/11/3.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_BACKEND_TYPE_H
#define FASTAIASSISTANT_ROOT_FAA_BACKEND_TYPE_H
#include <variant>

namespace faa::core::fdnn::backend {

     struct kTensorRT {
          enum class  Model {
               kDefault
          } model = Model::kDefault;
     };

     struct kOnnxRT {

          enum class  Model {
               kDefault ,
               KCpu ,
               kGpu
          } model = Model::kDefault;

     }; //TODO : 计划中

     using FaaDnnBackEndType = std::variant<kTensorRT, kOnnxRT>;

     const std::string kFaaTensorRT = "kTensorRT";
     const std::string kFaaOnnxRT   = "kOnnxRT";

     static  std::unordered_map<std::string, FaaDnnBackEndType> FaaDnnBackEndTypeMap{
          {kFaaTensorRT, kTensorRT{}},
          {kFaaOnnxRT, kOnnxRT{}},
     };
}
#endif //FASTAIASSISTANT_ROOT_FAA_BACKEND_TYPE_H