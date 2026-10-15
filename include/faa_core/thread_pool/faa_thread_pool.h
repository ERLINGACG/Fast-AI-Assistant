//
// Created by HP on 2026/6/30.
//

#ifndef FASTSCREEN_FAA_THREAD_POOL_H
#define FASTSCREEN_FAA_THREAD_POOL_H
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <vector>

namespace faa::core::thread_pool{
    class FAAThreadPool {

        int32_t max_threads=4;
        std::vector<std::thread> workers_;
        std::queue<std::function<void()>> tasks_;   // 任务队列
        std::mutex queue_mutex_;                    // 同步锁
        std::condition_variable condition_;         // 条件变量
        bool stop_=false;                           // 停止标志

        public:

        explicit FAAThreadPool(size_t thread_count = std::thread::hardware_concurrency())
        {
            if (thread_count == 0)
                thread_count = 1;  // 至少一个

            for (size_t i = 0; i < thread_count; ++i) {
                workers_.emplace_back([this] {
                    // 工作线程主循环
                    while (true) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(queue_mutex_);
                            // 等待条件：有任务 或 停止信号
                            condition_.wait(lock, [this] {
                                return stop_ || !tasks_.empty();
                            });
                            // 如果停止 且 任务队列为空，则退出线程
                            if (stop_ && tasks_.empty()) return;

                            task = std::move(tasks_.front());
                            tasks_.pop();
                        }
                        // 执行任务（不持有锁）
                        task();
                    }
                });
            }
        }
        template <typename F, typename... Args>
       auto enqueue(F&& f, Args&&... args)
           -> std::future<typename std::invoke_result_t<F, Args...>> {
                using return_type = typename std::invoke_result_t<F, Args...>;

                // 将任务包装为 packaged_task，以便返回 future
                auto task = std::make_shared<std::packaged_task<return_type()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                );

                std::future<return_type> result = task->get_future();

                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    if (stop_) {
                        throw std::runtime_error("enqueue on stopped FAAThreadPool");
                    }
                    // 将 task 加入队列（捕获 task 副本，保证生命周期）
                    tasks_.emplace([task]() { (*task)(); });
                }
                condition_.notify_one();
                return result;
        }
        ~FAAThreadPool() {
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    stop_ = true;
                }
                // 唤醒所有等待的线程
                condition_.notify_all();
                // 等待所有工作线程退出
                for (std::thread &worker : workers_) {
                    if (worker.joinable())
                        worker.join();
                }
        }
        FAAThreadPool(const FAAThreadPool&) = delete;
        FAAThreadPool& operator=(const FAAThreadPool&) = delete;

    };
}
#endif //FASTSCREEN_FAA_THREAD_POOL_H