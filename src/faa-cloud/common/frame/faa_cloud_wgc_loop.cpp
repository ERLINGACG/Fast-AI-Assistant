//
// Created by HP on 2026/8/31.
//

#include "faa_cloud/common/frame/faa_cloud_wgc_loop.h"
#include "faa_core/frame/private/faa_mat_impi.h"
#include "faa_core/wgc_cap/faa_wgc_cap.h"
#include "faa_cloud/common/frame/private/faa_cloud_frame_impl.h"
using faa::cloud::frame::Frames;
using faa::cloud::frame::FramesImpl;



void faa::cloud::frame::wgc_loop_v3(const std::function<bool()> call_back, const std::atomic<bool>& isRunning,
    core::cap::wgc::FAAWindowCapture& window_capture, Frames& frame_resources, int32_t fps, int32_t max_frame_rate)
{
    using clock = std::chrono::steady_clock;

    auto last_report = clock::now();
    uint64_t loop_count = 0;
    uint64_t frame_count = 0;

    while (call_back()) {
        ++loop_count;

        core::frame::FaaMat mat;
        window_capture >> mat;
        if (mat.impl()->mat.empty())
            std::this_thread::sleep_for(std::chrono::milliseconds(1));


        if (!mat.empty()) {
            ++frame_count;
            frame_resources.push(mat);
            frame_resources.keep(max_frame_rate);
        }

        auto now = clock::now();
        double elapsed = std::chrono::duration<double>(now - last_report).count();
        if (elapsed >= 1.0) {
            std::cout << "[wgc_loop] loop_fps=" << (loop_count / elapsed)
                      << "  frame_fps=" << (frame_count / elapsed)
                      << "  (loops=" << loop_count << ", frames=" << frame_count << ")\n";
            loop_count = 0;
            frame_count = 0;
            last_report = now;
        }

    }
}

