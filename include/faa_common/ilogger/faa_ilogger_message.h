//
// Created by HP on 2026/11/9.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_ILOGGER_MESSAGE_H
#define FASTAIASSISTANT_ROOT_FAA_ILOGGER_MESSAGE_H
#include <format>
#include <print>
#include <string_view>

namespace faa::common::ilogger::detail {



    static void message(
        std::string_view time_str,
        std::string_view color,
        std::string_view level,
        std::string_view body) {

        std::print("{} [ {}{}\033[0m ] {}{}\033[0m\n",
             time_str,        // 已自带 "[ ... ]"
             color, level,    // [ INFO ]，括号默认色，INFO 染色
             color, body
             );    // 消息染色

        std::fflush(stdout);

    }


}

#endif //FASTAIASSISTANT_ROOT_FAA_ILOGGER_MESSAGE_H