//
// Created by HP on 2026/8/31.
//

#include "faa_cloud/common/frame/faa_cloud_wgc_loop.h"

#include "faa-core/wgc_cap/faa_wgc_cap.h"

void faa::cloud::frame::wgc_loop(
    const std::atomic<bool>& isRunning,
    std::string target,
    Frames& frame_resources,
    const info::log::Ilog& ilog,
    int32_t fps,
    int32_t max_frame_rate){

    const core::cap::wgc::D3DDeviceContext  d3d_ctx;
    const core::cap::wgc::WGCRunTimeContext wgc_rt;
    auto window_capture =
        std::make_unique<core::cap::wgc::FAAWindowCapture>(target, ilog)->
        initD3d11(d3d_ctx).
        initWgcRtCtx(wgc_rt).startSession();

    while (isRunning.load()){
        core::cap::wgc::WgcTextureData wgc_texture_data{};
        cv::Mat frame{};
        window_capture.getFrame(wgc_texture_data,frame);
        if (!frame.empty()) {
             frame_resources.push(frame);
             frame_resources.keep(max_frame_rate);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/fps));
    }
}
