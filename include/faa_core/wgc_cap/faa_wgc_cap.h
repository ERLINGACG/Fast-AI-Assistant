//
// Created by HP on 2026/6/30.
//

#ifndef FASTSCREEN_FAA_WGC_CAP_H
#define FASTSCREEN_FAA_WGC_CAP_H
#define WIN32_LEAN_AND_MEAN
#include <utility>

// #include "faa_wgc_ctx.h"
#include "faa_wgc_data.h"
#include "info/ilog/ilog.h"
#include "faa_core/faa_export.h"
#include "faa_core/frame/faa_mat.h"

namespace faa::core::cap::wgc {

     struct WgcCapCore;

     using WgcCapCorePtr = std::unique_ptr<WgcCapCore>;

     class FAA_CPP_EXPORT FAAWindowCapture {



          info::log::Ilog log;

          WgcCapCorePtr core_ptr;

        public:
           FAAWindowCapture(std::string  target_window_name, info::log::Ilog log);
           ~FAAWindowCapture();                                  // 只声明，不 = default

           FAAWindowCapture(FAAWindowCapture&&) noexcept;        // 只声明
           FAAWindowCapture& operator=(FAAWindowCapture&&) noexcept;

           FAAWindowCapture(const FAAWindowCapture&) = delete;   // delete 可以留头文件
           FAAWindowCapture& operator=(const FAAWindowCapture&) = delete;
           void operator >> (frame::FaaMat& mat);



           FAAWindowCapture& initD3d11 ();
           FAAWindowCapture& initWgcRtCtx();
           FAAWindowCapture& setLogger(info::log::Ilog* log);




           FAAWindowCapture& startSession();

           bool getFrame(WgcTextureData& texture_data,frame::FaaMat& mat);
           bool open(info::log::Ilog* log);
           bool read(frame::FaaMat& mat);
           };
}
#endif //FASTSCREEN_FAA_WGC_CAP_H