//
// Created by HP on 2026/6/30.
//

#ifndef FASTSCREEN_FAA_WGC_CAP_H
#define FASTSCREEN_FAA_WGC_CAP_H
#define WIN32_LEAN_AND_MEAN
#include "faa_wgc_ctx.h"
#include "faa_wgc_data.h"
#include "info/ilog/ilog.h"
#include "faa_core/faa_export.h"

namespace faa::core::cap::wgc {

     class FAA_CPP_EXPORT FAAWindowCapture {

          std::string target_window_name;
          HWND        target_window_handle{};
          D3DDeviceContext d3d_device_context{};
          WGCRunTimeContext run_time_ctx{};
          bool is_init_=false;
          info::log::Ilog log;

        public:
           FAAWindowCapture(const std::string& target_window_name, const info::log::Ilog log) :
                    target_window_name(target_window_name), log(log)
            {};

           FAAWindowCapture& initD3d11(D3DDeviceContext device_context);
           FAAWindowCapture& initWgcRtCtx(WGCRunTimeContext run_time_ctx);
           FAAWindowCapture& startSession();
           FAAWindowCapture& getFrame(WgcTextureData& texture_data,cv::Mat& mat);

           bool is_init() const { return is_init_; }
     };
}
#endif //FASTSCREEN_FAA_WGC_CAP_H