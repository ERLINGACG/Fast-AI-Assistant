//
// Created by HP on 2026/8/5.
//

#ifndef FASTSCREEN_FAA_SERVICE_WGC_OPERATION_H
#define FASTSCREEN_FAA_SERVICE_WGC_OPERATION_H
#include "faa-core/wgc_cap/faa_wgc_cap.h"
#include "faa_service/resources/faa_service_resources.h"

namespace faa::service::operation{


    template<typename T>
    void wgc_loop_v2(T& t,event::Task task, resources::FaaServiceResources& resources,int32_t fps);


}
template<typename T>
   void faa::service::operation::wgc_loop_v2(T& t,event::Task task, resources::FaaServiceResources& resources,int32_t fps){
        auto wgc_resources=resources::WgcResources();
        auto log=t.getLog();
        if (resources.wgc_register_info.try_register(task.global_app,event::WGC)){
            log.info("wgc registration finished to: {}",task.global_app);
            return;
        }

        wgc_resources.
            window_capture=std::make_unique<core::cap::wgc::FAAWindowCapture>(task.global_app,log);

        wgc_resources.window_capture->initD3d11(wgc_resources.d3d_ctx).
                                      initWgcRtCtx(wgc_resources.wgc_rt).startSession();
    // startSession();

        if (!wgc_resources.window_capture->is_init()){
            log.error("wgc capture init failed: not found window to capture : {}",task.global_app);
            resources.wgc_register_info.unregister(task.global_app);
            return;
        }
        std::cout << std::flush;

        using core::cap::wgc::WgcTextureData;
        while (t.isRunning()){
            WgcTextureData wgc_texture_data{};
            cv::Mat frame{};
            wgc_resources.window_capture->getFrame(wgc_texture_data,frame);
            if (!frame.empty()){
                resources.frame_resources.push(task.global_app,frame);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000/fps));

        }
}
#endif //FASTSCREEN_FAA_SERVICE_WGC_OPERATION_H