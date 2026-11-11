//
// Created by HP on 2026/6/30.
//

#ifndef FASTSCREEN_FAA_WGC_CAP_H
#define FASTSCREEN_FAA_WGC_CAP_H
#define WIN32_LEAN_AND_MEAN
#include <utility>


#include "faa_wgc_data.h"
#include "faa_common/ilogger/faa_ilogger.h"
#include "info/ilog/ilog.h"
#include "faa_core/faa_export.h"
#include "faa_core/frame/faa_mat.h"

namespace faa::core::cap::wgc {

     struct WgcCapCore;

     using WgcCapCorePtr = std::unique_ptr<WgcCapCore>;

     class FAA_CPP_EXPORT FaaWindowCapture {



          // info::log::Ilog* log{};
          common::ilogger::FaaIlogger* logger = nullptr;

          WgcCapCorePtr core_ptr;
          bool isInitialized = false;

        public:
           // FaaWindowCapture(std::string  target_window_name, info::log::Ilog& log);
           FaaWindowCapture(std::string  target_window_name, common::ilogger::FaaIlogger* logger);
                     ~FaaWindowCapture();                                  // 只声明，不 = default

           FaaWindowCapture(FaaWindowCapture&&) noexcept;        // 只声明
           FaaWindowCapture& operator=(FaaWindowCapture&&) noexcept;

           FaaWindowCapture(const FaaWindowCapture&) = delete;   // delete 可以留头文件
           FaaWindowCapture& operator=(const FaaWindowCapture&) = delete;
           void operator >> (frame::FaaMat& mat);



           FaaWindowCapture& initD3d11 ();
           FaaWindowCapture& initWgcRtCtx();
           FaaWindowCapture& setLogger(common::ilogger::FaaIlogger* logger);




           FaaWindowCapture& startSession();

           bool getFrame(WgcTextureData& texture_data,frame::FaaMat& mat);
           bool open(common::ilogger::FaaIlogger* logger);
           bool read(frame::FaaMat& mat);
           };
}
#endif //FASTSCREEN_FAA_WGC_CAP_H