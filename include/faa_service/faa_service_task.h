//
// Created by HP on 2026/7/23.
//

#ifndef FASTSCREEN_FAA_SERVICE_TASK_H
#define FASTSCREEN_FAA_SERVICE_TASK_H
#include <map>
#include <string>

namespace faa::service::task{
    enum Tasks{

        NoTask        =0x00,
        RunWGC        =0x01,
        RunADB        =0x02,
        GetImg        =0x11,
        GetOCR        =0x12,

        Win32_NoDet   =0x13,
    };

    inline std::map<std::string,Tasks> task_operation_map{
        {"no_task",NoTask},
        {"run_wgc",RunWGC},
        {"run_adb",RunADB},
        {"get_img",GetImg},
        {"get_ocr",GetOCR},
        {"win32_no_det",Win32_NoDet},
    };
}
#endif //FASTSCREEN_FAA_SERVICE_TASK_H