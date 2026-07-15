//
// Created by HP on 2026/6/28.
//

#ifndef FASTSCREEN_ILOG_TIME_H
#define FASTSCREEN_ILOG_TIME_H
#include <chrono>
#include <string>

namespace info::log::time{
    static std::string get_time_str() {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);

        std::tm tm{};
        #ifdef _WIN32
                localtime_s(&tm, &t);   // Windows 安全版本
        #else
                localtime_r(&t, &tm);   // POSIX 安全版本 (Linux/macOS)
        #endif

        std::ostringstream oss;
        oss << std::put_time(&tm, "[%Y-%m-%d] [%H:%M:%S]");
        return oss.str();
    }
}
#endif //FASTSCREEN_ILOG_TIME_H