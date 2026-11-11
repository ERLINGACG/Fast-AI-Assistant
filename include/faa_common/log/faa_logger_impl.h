//
// Created by HP on 2026/11/9.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_LOGGER_IMPL_H
#define FASTAIASSISTANT_ROOT_FAA_LOGGER_IMPL_H
#include <utility>
#include "faa_logger.h"

namespace  faa::common::log {

    template <typename L>
    template <typename... Args>
    void FaaLogger<L>::info(Args&&... args) {
        logger.info(std::forward<Args>(args)...);
    }

    template <typename L>
    template <typename T, typename ... Args>
    void FaaLogger<L>::info(Args&&... args) {
        logger.template info<T>(std::forward<Args>(args)...);
    }

    template <typename L>
    template <typename... Args>
    void FaaLogger<L>::debug(Args&&... args) {
        logger.debug(std::forward<Args>(args)...);
    }

    template <typename L>
    template <typename T, typename ... Args>
    void FaaLogger<L>::debug(Args&&... args)
    {
        logger.template debug<T>(std::forward<Args>(args)...);
    }

    template <typename L>
    template <typename ... Args>
    void FaaLogger<L>::warn(Args&&... args){
        logger.warn(std::forward<Args>(args)...);
    }

    template <typename L>
    template <typename T, typename ... Args>
    void FaaLogger<L>::warn(Args&&... args)
    {
        logger.template warn<T>(std::forward<Args>(args)...);
    }

    template <typename L>
    template <typename ... Args>
    void FaaLogger<L>::error(Args&&... args) {
        logger.error(std::forward<Args>(args)...);
    }

    template <typename L>
    template <typename T, typename ... Args>
    void FaaLogger<L>::error(Args&&... args)
    {
        logger.template error<T>(std::forward<Args>(args)...);
    }
}
#endif //FASTAIASSISTANT_ROOT_FAA_LOGGER_IMPL_H