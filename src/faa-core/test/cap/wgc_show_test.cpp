//
// Created by HP on 2026/11/11.
//
#include "faa_core/cap/wgc_cap/faa_wgc_cap.h"
using faa::core::cap::wgc::FaaWindowCapture;

faa::common::ilogger::FaaIlogger logger{};



int main()
{
    FaaWindowCapture capture = FaaWindowCapture("明日方舟", &logger);
    if (!capture.open(&logger)) {
        return -1;
    }
    std::atomic run{true};
    std::vector<faa::core::frame::FaaMat> mat_queue;
    std::mutex mutex;
    auto thread_1 = std::thread([&]() {
        while (run.load()) {
            faa::core::frame::FaaMat mat;
            capture >> mat;
            if (mat.empty()){
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            {
                std::lock_guard lock(mutex);
                mat_queue.push_back(std::move(mat));
                if (mat_queue.size() > 3) {
                    mat_queue.erase(mat_queue.begin());
                }
            }
        }
        logger.info("thread_1 exit");
    });

    auto thread_2 = std::thread([&]() {
        while (run.load()) {

            auto frame = faa::core::frame::FaaMat();
            if (mat_queue.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            {
                std::lock_guard lock(mutex);
                if (!mat_queue.empty())
                {
                    auto& mat = mat_queue[0];
                    frame = std::move(mat);
                    mat_queue.erase(mat_queue.begin());
                }

            }
            if (!frame.show(1)) {
                run.store(false);
                break;
            }

        }
        logger.info("thread_2 exit");

    });

    thread_1.join();
    thread_2.join();
    return 0;
}