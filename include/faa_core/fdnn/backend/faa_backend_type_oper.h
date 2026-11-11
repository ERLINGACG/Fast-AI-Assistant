//
// Created by HP on 2026/11/5.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_BACKEND_TYPE_OPER_H
#define FASTAIASSISTANT_ROOT_FAA_BACKEND_TYPE_OPER_H

#include <functional>

#include "faa_core/fdnn/backend/faa_backend_type.h"

namespace faa::core::fdnn::backend
{
    inline void visit_to_type(
        const FaaDnnBackEndType type,
        std::function<void()> tensorrt,
        std::function<void()> onnxrt)
    {
        std::visit([tensorrt,onnxrt]<typename T0>(T0&& tag) {

         using T = std::decay_t<T0>;
         if constexpr (std::is_same_v<T, backend::kTensorRT>){
            tensorrt();
         }
         if constexpr (std::is_same_v<T, backend::kOnnxRT>) {
            onnxrt();
         }

    }, type);
    }
}
#endif //FASTAIASSISTANT_ROOT_FAA_BACKEND_TYPE_OPER_H