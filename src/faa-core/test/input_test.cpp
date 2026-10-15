//
// Created by HP on 2026/7/6.
//
#include <iostream>

#include "faa_core/input/windows_input.h"
#include "faa_core/win_utils/faa_win_utils.h"
#include <interception/interception.h>

void sendKeyAPost_HWND(HWND hWnd) {
    if (!hWnd) return;

    // 1. 按下
    PostMessage(hWnd, WM_KEYDOWN, 'A', 0x00000001);
    // 2. 字符消息（可选，很多游戏需要）
    PostMessage(hWnd, WM_CHAR, 'A', 0x00000001);
    // 3. 保持 50ms
    Sleep(50);
    // 4. 释放
    PostMessage(hWnd, WM_KEYUP, 'A', 0xC0000001);

    std::cout << "已发送 'A' 键（无扫描码）" << std::endl;
}
int main(){

    auto win=faa::core::win_utils::GetWindowHandle("Endfield");
    if(win!=nullptr){
        std::cout<<"find windows"<<std::endl;
    }else{
        std::cerr<<"not find windows"<<std::endl;
    }
    auto input=faa::input::windows::WindowsInput(win);
    // input.activity();

    input.simulateKeyPress({1000,"A"});
    input.simulateKeyPress({3000,"S"});
    input.simulateKeyPress({2000,"D"});
    input.simulateKeyPress({3000,"W"});
    // input.simulateMouseMoveToCenter();
    return 0;
}
