//
// Created by HP on 2026/8/5.
//

#ifndef FASTSCREEN_FAA_SERVICE_RESOURCES_WGC_H
#define FASTSCREEN_FAA_SERVICE_RESOURCES_WGC_H
#include <map>
#include <string>

#include "faa_service/service_event/faa_service_event_task.h"
#include "faa_core/wgc_cap/faa_wgc_cap.h"
namespace faa::service::resources{

    struct WgcRegisterInfo{
        std::mutex register_mutex;
        std::map<std::string, event::CapType> register_map;

        auto try_register(const std::string& global_app,event::CapType cap_type){
            std::lock_guard lock(register_mutex);
            if (!register_map.contains(global_app)){
                register_map[global_app]=cap_type;
                return false;
            }
            return true;
        }

        auto unregister(const std::string& global_app){
            std::lock_guard lock(register_mutex);
            register_map.erase(global_app);
        }

        // auto register_cap(const std::string& global_app,event::CapType cap_type){
        //     std::lock_guard lock(register_mutex);
        //     register_map[global_app] = cap_type;
        // }
    };

    struct WgcResources{
        std::unique_ptr<core::cap::wgc::FAAWindowCapture> window_capture;
        core::cap::wgc::D3DDeviceContext  d3d_ctx;
        core::cap::wgc::WGCRunTimeContext wgc_rt;
    };
}
#endif //FASTSCREEN_FAA_SERVICE_RESOURCES_WGC_H