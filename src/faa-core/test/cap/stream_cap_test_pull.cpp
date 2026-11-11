//
// Created by HP on 2026/11/9.
//
#include <mutex>
#include <queue>
#include <thread>

#include "faa_core/cap/stream_cap/faa_stream_cap.h"
using info::log::Ilog;

auto logger = Ilog([&]() {
        return info::log::IlogInit{
            true,
            false,
            true,
            true,
            true
        };
    });

using faa::core::cap::stream::FaaStreamCapture;
using faa::core::cap::stream::EndCodeOptParam;
using faa::core::cap::stream::DeCodeOptParam;
using faa::core::cap::stream::proto::Udp;
using faa::core::cap::stream::proto::Tcp;
int main() {
    FaaStreamCapture puller;
    DeCodeOptParam opt;
    // opt.codec    = "h264_cuvid";
    opt.lowDelay = true;                      // 开 AV_CODEC_FLAG_LOW_DELAY
    opt.extra["surfaces"] = "8";              // 解码缓冲从 25 降到 8
    opt.extra["deint"]    = "weave";          // 关反交错（如果源不是隔行）

    std::queue<faa::core::frame::FaaMat> frameQueue;
    std::mutex queueMtx;

    if (!puller.openPuller("127.0.0.1:8080", Udp{}, opt)) {
        std::printf("openPuller failed: %s\n", puller.lastError().c_str());
        return -1;
    }


    std::thread showFrameThread([&]() {
        auto localMat = faa::core::frame::FaaMat();
        while (true) {
            {
                std::lock_guard lock(queueMtx);
                if (!frameQueue.empty()) {

                    localMat = std::move(frameQueue.front());
                    frameQueue.pop();
                }else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
                }
            }

            if (!localMat.show(1,"get_mat")) {
                break;
            }
            if (localMat.empty())
            {
                continue;
            }
            if (!localMat.write("get_mat.png")) {
                break;
            }
        }
    });

    std::thread pullerThread([&]() {

        while (true) {
            {
                auto localMat = faa::core::frame::FaaMat();
                if (!puller.pull(localMat)) {
                  std::printf("pull failed: %s\n", puller.lastError().c_str());
                  break;
                }
                std::lock_guard lock(queueMtx);
                frameQueue.push(std::move(localMat));
                if (frameQueue.size() > 3) {
                   frameQueue.pop();
               }
            }
        }
    });
    pullerThread.join();
    showFrameThread.join();

    return 0;
}