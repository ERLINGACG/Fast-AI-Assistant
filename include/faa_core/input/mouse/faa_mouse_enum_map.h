//
// Created by HP on 2026/10/30.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_MOUSE_ENUM_MAP_H
#define FASTAIASSISTANT_ROOT_FAA_MOUSE_ENUM_MAP_H
#include <string>
#include <unordered_map>

#include "faa_mouse_enum.h"
namespace faa::core::input::mouse {

    static std::unordered_map<std::string, CoordinateSystem> coordinateSystemMap {
        {"screen", CoordinateSystem::kSCREEN},
        {"client", CoordinateSystem::kCLIENT},

    };

    static std::unordered_map<std::string, MouseMoveOper> moveOperMap {
        {"move",   MouseMoveOper::kMOVE},
        {"scroll", MouseMoveOper::kSCROLL},
        {"drag",   MouseMoveOper::kDRAG},
        {"click",  MouseMoveOper::kCLICK},
    };


    static std::unordered_map<std::string,ClickMode> clickMap {
        {"serial",   ClickMode::kSERIAL},
        {"parallel", ClickMode::kPARALLEL},

    };

    static std::unordered_map<std::string, MouseButton> buttonMap {
        {"left",   MouseButton::kLEFT},
        {"right",  MouseButton::kRIGHT},
        {"middle", MouseButton::kMIDDLE},
        {"x1",     MouseButton::kX1},
        {"x2",     MouseButton::kX2},
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_MOUSE_ENUM_MAP_H