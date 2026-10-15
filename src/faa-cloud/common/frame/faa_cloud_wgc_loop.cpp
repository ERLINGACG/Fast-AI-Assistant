//
// Created by HP on 2026/8/31.
//

#include "faa_cloud/common/frame/faa_cloud_wgc_loop.h"

#include "faa_core/wgc_cap/faa_wgc_cap.h"

void faa::cloud::frame::wgc_loop(
    const std::atomic<bool>& isRunning,
    const std::string& target,
    Frames& frame_resources,
    const info::log::Ilog& ilog,
    int32_t fps,
    int32_t max_frame_rate){

    const core::cap::wgc::D3DDeviceContext  d3d_ctx;
    const core::cap::wgc::WGCRunTimeContext wgc_rt;

    // if (!isRunning.load()) {
    //     return;
    // }
    // if (!isRunning.load()) {
    //     return;
    // }
    auto window_capture =
        std::make_unique<core::cap::wgc::FAAWindowCapture>(target, ilog);

    window_capture->initD3d11(d3d_ctx).initWgcRtCtx(wgc_rt).startSession();

    if (!window_capture->is_init()) {
        return;
    }
    while (isRunning.load()){
        core::cap::wgc::WgcTextureData wgc_texture_data{};
        cv::Mat frame{};
        window_capture->getFrame(wgc_texture_data,frame);
        if (!frame.empty()) {
             frame_resources.push(frame);
             frame_resources.keep(max_frame_rate);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/fps));
    }
}

void faa::cloud::frame::wgc_loop_v2(const std::atomic<bool>& isRunning,
    core::cap::wgc::FAAWindowCapture& window_capture, Frames& frame_resources, int32_t fps, int32_t max_frame_rate)
{
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

void faa::cloud::frame::wgc_loop_v3(std::function<bool()> call_back, const std::atomic<bool>& isRunning,
    core::cap::wgc::FAAWindowCapture& window_capture, Frames& frame_resources, int32_t fps, int32_t max_frame_rate)
{
    while (call_back()){
        // call_back();
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

