//
// Created by HP on 2026/11/8.
//
#include <mutex>
#include <queue>
#include <thread>

#include "faa_core/cap/wgc_cap/faa_wgc_cap.h"
#include "faa_core/cap/stream_cap/faa_stream_cap.h"
using faa::core::cap::wgc::FaaWindowCapture;
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
using faa::core::cap::stream::proto::Udp;
using faa::core::cap::stream::proto::Tcp;


int main() {
    std::string target_window_name = "Endfield";
    FaaStreamCapture pusher;
    EndCodeOptParam opt;
    opt.codec       = "h264_nvenc";
    opt.width       = 1280;
    opt.height      = 720;
    opt.fps         = 60;
    opt.bitrateKbps = 8000;           // 1920x1080p60 给 8Mbps 足够清晰
    opt.preset      = "p4";            // 【改】官方推荐：质量与延迟的平衡点
    opt.tune        = "hq";            // 高质量调优
    opt.gopSize     = opt.fps;         // 1秒 1个 I 帧

    opt.extra["rc"]           = "cbr"; // 【改】必须用 CBR，保证实时码率平稳
    opt.extra["cq"]           = "19";  // 恒定质量 19，画质极高
    opt.extra["rc-lookahead"] = "0";   // 【改】关闭前瞻，消除 0.3秒延迟，靠 CBR 保画质
    opt.extra["delay"]        = "0";   // 不排队
    opt.extra["zerolatency"]  = "1";   // 零延迟模式
    FaaWindowCapture capture = FaaWindowCapture(target_window_name, logger);
    if (!pusher.openPusher("127.0.0.1:8080", Udp{.listen = false}, opt)) {
        std::printf("openPusher failed: %s\n", pusher.lastError().c_str());
        return -1;
    }
    std::printf("pusher ready, run: ffplay udp://127.0.0.1:8080\n");


    capture.setLogger(&logger);
    if (!capture.open(&logger)) {
        return -1;
    }
    // if (!capture.)


    faa::core::frame::FaaMat lastMat;
    auto nextTick = std::chrono::steady_clock::now();
    const auto interval = std::chrono::microseconds(1000000 / opt.fps);


    std::queue<faa::core::frame::FaaMat> frameQueue;
    std::mutex queueMtx;


    // 2. 捕获线程
    std::thread getFrameThread([&]() {

        auto lastPrintTime = std::chrono::steady_clock::now();
            int frameCount = 0;

            while (true) {
                faa::core::frame::FaaMat mat;
                capture >> mat;

                if (!mat.empty()) {
                    {
                        std::lock_guard lock(queueMtx);
                        frameQueue.push(std::move(mat));
                        if (frameQueue.size() > 3) {
                            frameQueue.pop();
                        }
                    } // 锁释放

                    frameCount++; // 成功捕获一帧，计数加一

                    // 每累积 60 帧，计算一次真实 FPS
                    if (frameCount % 60 == 0) {
                        auto now = std::chrono::steady_clock::now();
                        // 计算这 60 帧花了的真实时间（秒）
                        auto elapsedSec = std::chrono::duration<double>(now - lastPrintTime).count();

                        // 平均帧率 = 60帧 / 耗时
                        double fps = 60.0 / elapsedSec;

                        std::cout << "Average FPS: " << fps << std::endl;

                        // 重置计时器，准备统计下 60 帧
                        lastPrintTime = now;
                    }
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }


                // 节流控制
                nextTick += interval;
                std::this_thread::sleep_until(nextTick);
            }
    });

  std::thread showFrameThread([&]() {
      faa::core::frame::FaaMat localMat;
 auto pushTick = std::chrono::steady_clock::now();

 while (true) {
     bool hasNewFrame = false;

     // 1. 极短时间拿数据
     {
         std::lock_guard lock(queueMtx);
         if (!frameQueue.empty()) {
             localMat = std::move(frameQueue.front());
             frameQueue.pop();
             hasNewFrame = true;
         }
     }

     // 2. 执行推流（放在锁外）
     if (hasNewFrame && !localMat.empty()) {
         if (!pusher.push(localMat)) {
             std::printf("push failed: %s\n", pusher.lastError().c_str());
             // stop_flag = true; // 推流失败，通知捕获线程退出
             break;
         }
     } else {
         // 【关键修复】如果没有新帧，必须让出 CPU，防止空转！
         std::this_thread::sleep_for(std::chrono::milliseconds(2));
     }

     // 3. 强制推流端维持 60fps 节奏，防止推得太快打爆网络
     pushTick += interval;
     std::this_thread::sleep_until(pushTick);
 }
    });
    showFrameThread.join();
    getFrameThread.join();



    // pusher.closePusher();
    return 0;
}
