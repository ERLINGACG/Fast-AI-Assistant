//
// Created by HP on 2026/7/27.
//

#ifndef FASTSCREEN_FAA_SERVICE_RESOURCES_H
#define FASTSCREEN_FAA_SERVICE_RESOURCES_H

#include "faa_service_resources_thread.h"
#include "faa_service_resources_frames.h"
#include "faa_service_resources_input.h"
#include "faa_service_resources_ocr.h"
#include "faa_service_resources_wgc.h"
#include "faa_service/runtime/faa_service_runtime.h"
#undef ERROR
namespace faa::service::resources{




    struct FaaServiceResources{

        thread::Threads     threads{};
        thread::ThreadPools thread_pools{};

        frame ::FrameResources frame_resources{};
        WgcRegisterInfo wgc_register_info{};
        ocr::FaaOcrDnnResource ocr_dnn_resource{};
        input::FaaInputResources input_resources{};
        // rt::FaaServiceOcrResources ocr_resources2{};

        bool init(){
            return thread_pools.init();
        }
    };
}
#endif //FASTSCREEN_FAA_SERVICE_RESOURCES_H