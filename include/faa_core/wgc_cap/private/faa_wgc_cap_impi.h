//
// Created by HP on 2026/10/20.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_WGC_CAP_IMPI_H
#define FASTAIASSISTANT_ROOT_FAA_WGC_CAP_IMPI_H
#include "faa_core/wgc_cap/faa_wgc_ctx.h"
#include "info/ilog/ilog.h"

namespace faa::core::cap::wgc {

    struct WgcCapCore {

        std::string target;
        HWND        handle{};
        D3DDeviceContext d3d_device_context{};
        WGCRunTimeContext run_time_ctx{};
        bool is_init_=false;
        info::log::Ilog* log;


        bool find();
        bool isSuccess() const;


    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_WGC_CAP_IMPI_H