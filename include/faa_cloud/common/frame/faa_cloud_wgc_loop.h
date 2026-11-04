//
// Created by HP on 2026/8/31.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CLOUD_WGC_LOOP_H
#define FASTAIASSISTANT_ROOT_FAA_CLOUD_WGC_LOOP_H
#include <functional>

#include "faa_cloud_frame.h"
#include "faa_core/faa_export.h"
#include "faa_core/wgc_cap/faa_wgc_cap.h"

namespace faa::cloud::frame{





    void FAA_CPP_EXPORT wgc_loop_v3(std::function<bool()> call_back,
                                const std::atomic<bool>& isRunning,
                                core::cap::wgc::FAAWindowCapture& window_capture,
                                Frames& frame_resources,
                                int32_t fps,int32_t max_frame_rate);

}
#endif //FASTAIASSISTANT_ROOT_FAA_CLOUD_WGC_LOOP_H
