//
// Created by HP on 2026/10/20.
//
#include "faa_core/cap/wgc_cap/private/faa_wgc_cap_impi.h"

#include "faa_core/win_utils/faa_win_utils.h"
using faa::core::cap::wgc::WgcCapCore;

bool WgcCapCore::find(common::ilogger::FaaIlogger* logger) {

    auto* log = logger;
    if (target.empty()) {
        if (log) log->error("the target is empty");
        return false;
    }
    handle=win_utils::GetWindowHandle(target);

    if (handle == nullptr) {
        if (log) log->debug("try find to windows by class");
        handle = win_utils::GetWindowHandleByClass(target);
    }

    if (handle == nullptr) {
        if (log) log->debug("try find to windows by name");
        handle = win_utils::GetWindowHandleByName(target);
    }

    if (handle == nullptr) {
        if (log) log->error("this target_window_handle query failed ");
        return false;
    }

    return true;
}



