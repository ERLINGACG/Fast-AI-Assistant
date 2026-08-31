//
// Created by HP on 2026/6/30.
//
#include "faa-core/win_utils/faa_win_utils.h"
struct EnumContext {
    std::wstring target;  // 目标窗口名（宽字符）
    HWND result = nullptr;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<EnumContext*>(lParam);
    if (!ctx) return TRUE;

    // 获取窗口标题（宽字符版本）
    wchar_t window_text[256];
    if (GetWindowTextW(hwnd, window_text, 256) > 0) {
        std::wstring current(window_text);
        // 不区分大小写的完全匹配
        if (_wcsicmp(current.c_str(), ctx->target.c_str()) == 0) {
            ctx->result = hwnd;
            return FALSE;  // 找到窗口，停止枚举
        }
    }
    return TRUE;  // 继续枚举
}



HWND faa::core::win_utils::GetWindowHandle(const std::string& window_name) {

    if (window_name.empty()) return nullptr;

    // 将 std::string (ANSI) 转换为 std::wstring (Unicode)
    int len = MultiByteToWideChar(CP_ACP, 0, window_name.c_str(), -1, nullptr, 0);
    if (len == 0) return nullptr;
    std::wstring wname(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, window_name.c_str(), -1, &wname[0], len);
    // 移除末尾的额外空字符
    if (!wname.empty() && wname.back() == L'\0')
        wname.pop_back();

    EnumContext ctx;
    ctx.target = wname;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&ctx));
    return ctx.result;
}

HWND faa::core::win_utils::GetWindowHandleByClass(const std::string& class_name) {
    if (class_name.empty()) return nullptr;

    int len = MultiByteToWideChar(CP_UTF8, 0, class_name.c_str(), -1, nullptr, 0);
    if (len == 0) return nullptr;
    std::wstring wname(len, L'\0');
    MultiByteToWideChar(
        CP_UTF8, 0,
        class_name.c_str(), -1, &wname[0], len
        );
    if (!wname.empty() && wname.back() == L'\0') wname.pop_back();

    return FindWindowW(wname.c_str(), nullptr); // 直接通过类名查找
}

HWND faa::core::win_utils::GetWindowHandleByName(const std::string& window_name)
{
    if (window_name.empty()) return nullptr;

    int len = MultiByteToWideChar(CP_UTF8, 0, window_name.c_str(), -1, nullptr, 0);
    if (len == 0) return nullptr;
    std::wstring wname(len, L'\0');
    MultiByteToWideChar(
        CP_UTF8, 0,
        window_name.c_str(), -1, &wname[0], len
        );
    if (!wname.empty() && wname.back() == L'\0') wname.pop_back();

   return FindWindowW(nullptr, wname.c_str());

}
