//
// Created by HP on 2026/11/9.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_ILOGER_H
#define FASTAIASSISTANT_ROOT_FAA_ILOGER_H
#include <string>
#include <format>
#include <mutex>
#include <queue>
#include <variant>

#include "faa_ilogger_utils.h"


namespace faa::common::ilogger {


    struct kFaaInfo {
        std::string level = "INFO ";
        std::string color = "\033[32m";
    };

    struct kFaaWarn  {
        std::string level = "WARN ";
        std::string color = "\033[33m";
    };

    struct kFaaError {
        std::string level = "ERROR";
        std::string color = "\033[31m";
    };

    struct kFaaDebug {
        std::string level = "DEBUG";
        std::string color = "\033[36m";
    };

    using kFaaLevel = std::variant<kFaaInfo, kFaaWarn, kFaaError, kFaaDebug>;

    struct FaaLogStruct {

        kFaaLevel   level;
        std::string time_str;
        // std::string master;
        std::string body;
    };

}
namespace faa::common::ilogger {

    struct FaaIloggerEnabled {

        bool enabled_debug = true;
        bool enabled_warning = true;
        bool enabled_error = true;
        bool enabled_info = true;
    };



    class FaaIlogger {

            FaaIloggerEnabled enabled;
            std::atomic<bool> running = true;
            std::queue<FaaLogStruct> log_queue;

            std::mutex  log_mutex;
            std::thread log_thread;

    public:
        explicit FaaIlogger(
                FaaIloggerEnabled e = FaaIloggerEnabled{}
            );


        ~FaaIlogger();

        template<typename ... Args> void info(
                std::string_view fmt,
                const Args&... args
            );



        template<typename ... Args> void warn(
                std::string_view fmt,
                const Args&... args
            );

        template<typename ... Args> void error(
                std::string_view fmt,
                const Args&... args
            );

        template<typename ... Args> void debug(
                std::string_view fmt,
                const Args&... args
            );




    };


}




#include "faa_ilogger_impl.h"
#endif //FASTAIASSISTANT_ROOT_FAA_ILOGER_H