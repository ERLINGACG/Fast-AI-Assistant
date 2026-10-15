//
// Created by HP on 2026/8/31.
//

#ifndef FASTSCREEN_FAA_CLOUD_WGC_LOOP_H
#define FASTSCREEN_FAA_CLOUD_WGC_LOOP_H
#include "faa_cloud_frame.h"
#include "faa_core/faa_export.h"
#include "faa_core/wgc_cap/faa_wgc_cap.h"
#include "info/ilog/ilog.h"

namespace faa::cloud::frame{

    void FAA_CPP_EXPORT wgc_loop(const std::atomic<bool>& isRunning,
                                     const std::string& target,Frames& frame_resources,
                                     const info::log::Ilog& ilog,
                                     int32_t fps,int32_t max_frame_rate);

    void FAA_CPP_EXPORT wgc_loop_v2(const std::atomic<bool>& isRunning,
                                 core::cap::wgc::FAAWindowCapture& window_capture,
                                 Frames& frame_resources,
                                 int32_t fps,int32_t max_frame_rate);


    void FAA_CPP_EXPORT wgc_loop_v3(std::function<bool()> call_back,
                                const std::atomic<bool>& isRunning,
                                core::cap::wgc::FAAWindowCapture& window_capture,
                                Frames& frame_resources,
                                int32_t fps,int32_t max_frame_rate);

}
#endif //FASTSCREEN_FAA_CLOUD_WGC_LOOP_H