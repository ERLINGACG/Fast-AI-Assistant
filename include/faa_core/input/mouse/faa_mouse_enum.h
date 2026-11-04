//
// Created by HP on 2026/10/29.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_MOUSE_ENUM_H
#define FASTAIASSISTANT_ROOT_FAA_MOUSE_ENUM_H
namespace faa::core::input::mouse {

    enum class CoordinateSystem {
        kSCREEN, // 屏幕坐标
        kCLIENT,  // 客户端坐标
        kVIRTUAL, // 虚拟坐标
    };

    enum class MouseMoveOper {
        kMOVE,    // 移动
        kSCROLL,  // 滚动
        kDRAG,    // 拖拽
        kCLICK,   // 点击
    };

    enum class ClickMode {
        kSERIAL, // 串行点击
        kPARALLEL, // 并行点击
    };

    enum class MouseButton {
        kLEFT,    // 左键
        kRIGHT,   // 右键
        kMIDDLE,  // 中键
        kX1,      // X1键
        kX2,      // X2键
    };



}
#endif //FASTAIASSISTANT_ROOT_FAA_MOUSE_ENUM_H