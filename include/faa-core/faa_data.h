//
// Created by HP on 2026/6/29.
//

#ifndef FASTSCREEN_FS_DATA_H
#define FASTSCREEN_FS_DATA_H
#include <cstdint>
#include <string>

namespace faa::core::data {

    struct RequestData{
       int64_t timestamp;
       std::string model;
       std::string target_key;
    };
}
#endif //FASTSCREEN_FS_DATA_H