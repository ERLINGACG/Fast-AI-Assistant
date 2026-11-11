//
// Created by HP on 2026/11/9.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_ILOGER_UTILS_H
#define FASTAIASSISTANT_ROOT_FAA_ILOGER_UTILS_H
#include <chrono>
#include <source_location>
#include <string>

namespace  faa::common::ilogger {



    static std::string get_time_str() {
        // ① 线程局部缓存：秒级前缀每秒只算一次
        thread_local int64_t cached_sec = -1;
        thread_local char    cached_prefix[24];   // "2026-11-10T18:38:09"
        thread_local int     cached_len = 0;

        auto now = std::chrono::system_clock::now();
        int64_t ms_total = std::chrono::duration_cast<std::chrono::milliseconds>(
                               now.time_since_epoch()).count();
        int64_t sec = ms_total / 1000;
        int     ms  = static_cast<int>(ms_total % 1000);

        if (sec != cached_sec) {                  // 秒变了才调 localtime
            cached_sec = sec;
            auto t = sec;
            std::tm tm_buf{};
#ifdef _WIN32
            localtime_s(&tm_buf, &t);
#else
            localtime_r(&t, &tm_buf);
#endif
            cached_len = std::snprintf(cached_prefix, sizeof(cached_prefix),
                "%04d-%02d-%02dT%02d:%02d:%02d",
                tm_buf.tm_year + 1900, tm_buf.tm_mon + 1, tm_buf.tm_mday,
                tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec);
        }

        // ② 组装：reserve 一次分配，毫秒手动拼（比 snprintf 快 5~10 倍）
        std::string result;
        result.reserve(32);                       // 一次 malloc，避免 realloc
        result += "[ ";
        result.append(cached_prefix, cached_len);
        result += '.';
        result += char('0' + ms / 100);
        result += char('0' + (ms / 10) % 10);
        result += char('0' + ms % 10);
        result += " ]";
        return result;
    }




}
#endif //FASTAIASSISTANT_ROOT_FAA_ILOGER_UTILS_H