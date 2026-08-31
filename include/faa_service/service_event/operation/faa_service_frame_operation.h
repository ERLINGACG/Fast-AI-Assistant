//
// Created by HP on 2026/8/6.
//

#ifndef FASTSCREEN_FAA_SERVICE_FRAME_OPERATION_H
#define FASTSCREEN_FAA_SERVICE_FRAME_OPERATION_H
#include <opencv2/imgcodecs.hpp>

#include "faa_service/resources/faa_service_resources.h"
#include "faa_service/service_event/faa_service_event_task.h"

namespace faa::service::operation{

    inline auto get_frame(resources::FaaServiceResources& resources,cv::Mat& frame,std::string global_app){
         if (!resources.frame_resources.peek(global_app,frame)){
             return false;
         }
         return true;
    }

    inline auto save_frame(resources::FaaServiceResources& resources, const std::string& global_app, const std::string& file_path){
        cv::Mat frame{};
        if (!get_frame(resources,frame,global_app)){
            return false;
        }

        cv::imwrite("test.jpg",frame);
        return true;
    }


}
#endif //FASTSCREEN_FAA_SERVICE_FRAME_OPERATION_H