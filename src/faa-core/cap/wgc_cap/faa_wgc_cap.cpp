//
// Created by HP on 2026/6/30.
//
#include "faa_core/cap/wgc_cap/faa_wgc_cap.h"
#include "faa_core/win_utils/faa_win_utils.h"
#include <windows.graphics.directx.direct3d11.interop.h>
#include <Windows.Graphics.Capture.Interop.h>

#include <utility>
#include "faa_core/cap/wgc_cap/private/faa_wgc_cap_impi.h"

using faa::core::cap::wgc::FaaWindowCapture;
using faa::core::cap::wgc::WgcCapCore;




FaaWindowCapture::FaaWindowCapture(std::string target_window_name, common::ilogger::FaaIlogger* logger) {
    this->core_ptr         = std::make_unique<WgcCapCore>();
    this->core_ptr->target = std::move(target_window_name);
    this->logger = logger;
}

FaaWindowCapture::~FaaWindowCapture() = default;
FaaWindowCapture::FaaWindowCapture(FaaWindowCapture&&) noexcept = default;
FaaWindowCapture& FaaWindowCapture::operator=(FaaWindowCapture&&) noexcept = default;


void FaaWindowCapture::operator >> (frame::FaaMat& mat)
{
    this->read(mat);
}


FaaWindowCapture& FaaWindowCapture::setLogger(common::ilogger::FaaIlogger* logger) {
    this->logger = logger;
    return *this;
}

FaaWindowCapture& FaaWindowCapture::startSession(){

    auto* log = logger;
    auto& ctx = core_ptr->run_time_ctx;
    if (log) log->debug("startSession");

    if (!this->isInitialized) {
        if (log) log->error("is_init failed");
        return *this;
    }
    if (!ctx.framePool) {
        if (log) log->error("run_time_ctx.framePool is nullptr");
        return *this;
    }

    ctx.session = ctx.framePool.CreateCaptureSession(ctx.captureItem);
    ctx.session.StartCapture();
    if (log) log->debug("startSession success");
    return *this;

}





