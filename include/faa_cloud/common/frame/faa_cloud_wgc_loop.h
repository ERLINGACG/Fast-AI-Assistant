//
// Created by HP on 2026/8/31.
//

#ifndef FASTSCREEN_FAA_CLOUD_WGC_LOOP_H
#define FASTSCREEN_FAA_CLOUD_WGC_LOOP_H
#include "faa_cloud_frame.h"
#include "faa-core/faa_export.h"
#include "info/ilog/ilog.h"

namespace faa::cloud::frame{

    void FAA_CPP_EXPORT wgc_loop(const std::atomic<bool>& isRunning,
                                     std::string target,Frames& frame_resources,
                                     const info::log::Ilog& ilog,
                                     int32_t fps,int32_t max_frame_rate);

}
#endif //FASTSCREEN_FAA_CLOUD_WGC_LOOP_H