//
// Created by HP on 2026/11/9.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_ILOGER_IMPL_H
#define FASTAIASSISTANT_ROOT_FAA_ILOGER_IMPL_H
#include <string_view>
#include "faa_ilogger_message.h"

namespace faa::common::ilogger {



    inline FaaIlogger::FaaIlogger(const FaaIloggerEnabled e) : enabled(e) {
        log_thread = std::thread([this] {
            while (running || !log_queue.empty()) {   // 队列没空就继续干
                FaaLogStruct log;
                bool got = false;
                {
                    std::lock_guard lock(log_mutex);   // 锁内读 empty
                    if (!log_queue.empty()) {
                        log = std::move(log_queue.front());
                        log_queue.pop();
                        got = true;
                    }
                }
                if (!got) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));  // 空就睡 1ms
                    continue;
                }
                auto [color, level] = std::visit([](const auto& l) {
                    return std::pair<std::string, std::string>{l.color, l.level};
                }, log.level);

                detail::message(log.time_str, color, level, log.body);
            }
        });
    }

    inline FaaIlogger::~FaaIlogger() {
        running = false;
        if (log_thread.joinable()) log_thread.join();
        std::fflush(stdout);
    }


    template <typename ... Args>
    void FaaIlogger::info(std::string_view fmt, const Args&... args)
    {
        if (!enabled.enabled_info) return;

        auto log_struct = FaaLogStruct{

            .level = kFaaInfo(),
            .time_str = get_time_str(),
            .body = std::vformat(fmt, std::make_format_args(args...))
        };
        std::lock_guard lock(log_mutex);
        log_queue.push(std::move(log_struct));

    }



    template <typename ... Args>
    void FaaIlogger::warn(std::string_view fmt, const Args&... args) {
        if (!enabled.enabled_warning) return;
        auto log_struct = FaaLogStruct{

            .level = kFaaWarn(),
            .time_str = get_time_str(),
            .body = std::vformat(fmt, std::make_format_args(args...))
        };
        std::lock_guard lock(log_mutex);
        log_queue.push(std::move(log_struct));
    }

    template <typename ... Args>
    void FaaIlogger::error(std::string_view fmt, const Args&... args) {
        if (!enabled.enabled_error) return;
        auto log_struct = FaaLogStruct{

            .level = kFaaError(),
            .time_str = get_time_str(),

            .body = std::vformat(fmt, std::make_format_args(args...))
        };
        std::lock_guard lock(log_mutex);
        log_queue.push(std::move(log_struct));
    }

    template <typename ... Args>
    void FaaIlogger::debug(std::string_view fmt, const Args&... args) {
        if (!enabled.enabled_debug) return;
        auto log_struct = FaaLogStruct{

            .level = kFaaDebug(),
            .time_str = get_time_str(),
            .body = std::vformat(fmt, std::make_format_args(args...))
        };
        std::lock_guard lock(log_mutex);
        log_queue.push(std::move(log_struct));
    }
}
#endif //FASTAIASSISTANT_ROOT_FAA_ILOGER_IMPL_H