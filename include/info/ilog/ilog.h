//
// Created by HP on 2026/6/28.
//

#ifndef FASTSCREEN_ILOG_H
#define FASTSCREEN_ILOG_H
#include "ilog_time.h"
#include <iostream>
namespace info::log{

    struct IlogInit {
        bool enabled_info=true;
        bool enabled_debug=true;
        bool enabled_trace=true;
        bool enabled_error=true;
        bool enabled_endl=true;
    };

    class Ilog {
        IlogInit init{};
        public:
            template<typename T>
            explicit Ilog(T init) : init(init()){}

            void message(const std::string& log_level,const std::string& fmt) const {
                std::cout << time::get_time_str() << " "<<log_level << ": " << fmt;
                if (init.enabled_endl)
                    std::cout << '\n';
            }

            template<typename ... Args>
            void info(const std::string_view fmt, const Args&... args) {
                if (!init.enabled_info) return;
                auto str = std::vformat(fmt, std::make_format_args(args...));
                message("INFO",str);
            }

            template<typename ... Args>
            void debug(const std::string_view fmt, const Args&... args){
                if (!init.enabled_debug) return;
                auto str = std::vformat(fmt, std::make_format_args(args...));
                message("DEBUG",str);
            }

            template<typename ... Args>
            void trace(const std::string_view fmt, const Args&... args){
                if (!init.enabled_trace) return;
                auto str = std::vformat(fmt, std::make_format_args(args...));
                message("TRACE",str);
            }

             template<typename ... Args>
             void error(const std::string_view fmt, const Args&... args){
                if (!init.enabled_error) return;
                auto str = std::vformat(fmt, std::make_format_args(args...));
                message("ERROR",str);
             }

             template<typename ... Args>
             void warn(const std::string_view fmt, const Args&... args){
                if (!init.enabled_error) return;
                auto str = std::vformat(fmt, std::make_format_args(args...));
                message("WARN",str);
             }
    };
}
#endif //FASTSCREEN_ILOG_H