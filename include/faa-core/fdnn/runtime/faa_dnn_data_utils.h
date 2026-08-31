//
// Created by HP on 2026/7/19.
//

#ifndef FASTSCREEN_FAA_DNN_DATA_UTILS_H
#define FASTSCREEN_FAA_DNN_DATA_UTILS_H
#include <any>
#include <functional>
#include <map>
#include <string>

namespace faa::fdnn::rt::data_utils{
    inline std::map<std::string, std::function<auto(void*&, std::any&)>> map = {
        {"fp32", [](void*& src, std::any& dst)  { dst = static_cast<float*>(src); }},
        {"int32", [](void*& src, std::any& dst) { dst = static_cast<int32_t*>(src); }},
        {"int8",  [](void*& src, std::any& dst) { dst = static_cast<int8_t*>(src); }}
    };
}
#endif //FASTSCREEN_FAA_DNN_DATA_UTILS_H