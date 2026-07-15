//
// Created by HP on 2026/6/30.
//

#ifndef FASTSCREEN_FAA_WGC_PARAM_H
#define FASTSCREEN_FAA_WGC_PARAM_H
#include "faa_service/faa_net_work.h"

namespace faa::core::cap::wgc{

    struct WGCRunTimeCtxParam {
        bool use_monitor=true;
        HWND window_handle=nullptr;
        // dylog::DynamicLogger logger;
    };
}
#endif //FASTSCREEN_FAA_WGC_PARAM_H