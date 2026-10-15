//
// Created by HP on 2026/10/11.
//

#ifndef FASTSCREEN_FAA_COMMAND_TOKENZ_H
#define FASTSCREEN_FAA_COMMAND_TOKENZ_H
#include <ranges>
#include <string>
#include <vector>

#include "info/ilog/ilog.h"

namespace faa::cloud::command {

    enum class LexState {
        outSide,
        inDoubleQuote,
        inSingleQuote,
        inEscape,
    };

    void tokenize(const std::string& str,std::vector<std::string>& tokens,info::log::Ilog* log);

}

inline void faa::cloud::command::tokenize(
    const std::string& str,
    std::vector<std::string>& tokens ,
    info::log::Ilog* log
) {

    std::string command = str;
    if (command.empty()||command[0] != '/') {
        return;
    }
    command.erase(0,1);
    tokens.clear();
    const auto sp = command.find(' ');
    const std::string name = (sp == std::string::npos)
                     ? command                      // 没有空格，整个就是命令名
                     : command.substr(0, sp);// 截到空格前


    tokens.push_back(name);
    command.erase(0,sp);
    if (command.empty()) {
        return;
    }
    if (log) log->debug("cmd main : {}",name);

    std::string token;
    bool has_content = false;

    LexState state = LexState::outSide;
    LexState escape_from = LexState::outSide;   // 转义完回到哪

    for (const char c : command) {
        switch (state) {

        case LexState::outSide:
            if (c == ' ' || c == '\t') {
                if (has_content) {
                    tokens.push_back(token);
                    token.clear();
                    has_content = false;
                }
            } else if (c == '"') {
                state = LexState::inDoubleQuote;
                has_content = true;
            } else if (c == '\'') {
                state = LexState::inSingleQuote;
                has_content = true;
            } else if (c == '\\') {
                state = LexState::inEscape;
                escape_from = LexState::outSide;
                has_content = true;
            } else {
                token += c;
                has_content = true;
            }
            break;

        case LexState::inDoubleQuote:
            if (c == '\\') {
                state = LexState::inEscape;
                escape_from = LexState::inDoubleQuote;
            } else if (c == '"') {
                state = LexState::outSide;
            } else {
                token += c;
            }
            break;

        case LexState::inSingleQuote:
            if (c == '\'') {
                state = LexState::outSide;
            } else {
                token += c;      // 单引号内什么都不解析
            }
            break;

        case LexState::inEscape:
            switch (c) {
        case 'n':  token += '\n'; break;
        case 't':  token += '\t'; break;
        case 'r':  token += '\r'; break;
        case '\\': token += '\\'; break;
        case '"':  token += '"';  break;
        case '\'': token += '\''; break;
        case ' ':  token += ' ';  break;
        default:   token += c;    break;
            }
            state = escape_from;   // 回到转义前
            break;
        }
    }
    if (has_content) tokens.push_back(token);

    for (const auto& t : tokens) {
            if (log) log->debug("cmd token : {}",t);
    }

}
#endif //FASTSCREEN_FAA_COMMAND_TOKENZ_H