//
// Created by HP on 2026/11/9.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_LOGGER_H
#define FASTAIASSISTANT_ROOT_FAA_LOGGER_H
namespace faa::common::log {

    template <typename L>
    class  FaaLogger {

        L logger;
        public:
            explicit FaaLogger(L logger) : logger(std::move(logger)) {}

            template <typename... Args>
            void info(Args&&... args);

            template <typename T, typename... Args>
            void info(Args&&... args);

            template <typename... Args>
            void debug(Args&&... args);

            template <typename T, typename... Args>
            void debug(Args&&... args);

            template <typename... Args>
            void warn(Args&&... args);

            template <typename T, typename... Args>
            void warn(Args&&... args);

            template <typename... Args>
            void error(Args&&... args);

            template <typename T, typename... Args>
            void error(Args&&... args);

            auto& getLogger() { return logger; }
    };
}
#include "faa_logger_impl.h"
#endif //FASTAIASSISTANT_ROOT_FAA_LOGGER_H