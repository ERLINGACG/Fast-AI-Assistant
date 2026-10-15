//
// Created by HP on 2026/6/29.
//

#ifndef FASTSCREEN_FS_DATA_H
#define FASTSCREEN_FS_DATA_H
#include <cstdint>
#include <string>

#include "input/windows_input.h"

namespace faa::core::data {

    struct RequestData{
       int64_t timestamp;
       std::string model;
       std::string target_key;
       std::string task_type;

    };

    struct EvenTStruct{
        std::string timestamp;
        std::string target_key;
        std::string task_type;

        struct input{
            std::string key_name;
            int32_t delay_time;
            int32_t move_x=-1;
            int32_t move_y=-1;
            bool    is_click=false;
        };

        std::vector<input> input_list;

    };
}
#endif //FASTSCREEN_FS_DATA_H