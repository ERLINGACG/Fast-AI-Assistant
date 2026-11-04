//
// Created by HP on 2026/10/17.
//
#include "faa_cloud/microservices/general/service/faa_cloud_general.h"
#include "faa_core/feature/faa_cv_hough_circle.h"
using faa::cloud::microservices::general::FaaCloudGeneralServices;
#include "faa_cloud/common/frame/private/faa_cloud_frame_impl.h"

void FaaCloudGeneralServices::initSurfFeature(info::log::Ilog* log) {
    if (this->logger) {
        this->surf_feature.setLogger(this->logger);
    }else {
        this->surf_feature.setLogger(log);
    }
    using core::fcv::resources::FaaCvResource;


}

void FaaCloudGeneralServices::addSurfOper(info::log::Ilog* log) {
    using core::fcv::feature::FaaSurfParam;
    using core::fcv::feature::FaaSurfResult;

    this->operat.push("surf_feature",
        [this,log]
                (net_work::FaaCloudNetworkLoad& load,sockaddr_in response_addr){
                        auto param = FaaSurfParam{}.build(load.body().data);

                        // this->surf_feature.runSurfToGPU()
                        cv::Mat frame;
                        FaaSurfResult result;
                        this->cap.frame_resources.impl()->peek(frame);
                        if (frame.empty()){
                            if (log) log->warn("frame is empty");
                            load.setBody({
                                {"error","frame is empty"}
                            });
                            this->network.get().sendLoad(load,response_addr);
                            return -1;
                        }
                        try {
                            this->surf_feature.runSurfToGPU(
                                frame,
                                param,
                                this->cv_resource,
                                result);
                            load.setBody(result.serialize());
                        }catch(std::exception& e) {
                            if (log) log->error("FaaCloudGeneralServices::addSurfOper: runSurfToGPU error: {}",e.what());
                            load.setBody({
                                {"error",e.what()}
                            });
                        }

                        this->network.get().sendLoad(load,response_addr);

                        return 0;
                }
    );
}

void FaaCloudGeneralServices::addHoughCircleOper() {
    using core::fcv::feature::FaaHoughCircleParam;
    using core::fcv::feature::FaaCircleResult;
    using core::fcv::feature::houghCircleDetect;
    using core::fcv::feature::houghCircleNMS;
    this->operat.push("hough_circle",
       [this]
               (net_work::FaaCloudNetworkLoad& load,sockaddr_in response_addr){
                     auto param = FaaHoughCircleParam{}.build(load.body().data);

                     cv::Mat frame;
                     this->cap.frame_resources.impl()->peek(frame);
                     if (frame.empty()){
                         if (this->logger) this->logger->warn("frame is empty");
                         load.setBody({
                             {"error","frame is empty"}
                         });
                         this->network.get().sendLoad(load,response_addr);
                         return -1;
                     }
                     try
                     {
                         auto result = houghCircleDetect(frame,param);
                         houghCircleNMS(
                             result
                         );
                         load.setBody(result.serialize());
                     }catch(std::exception& e) {
                         if (this->logger) this->logger->error("FaaCloudGeneralServices::addHoughCircleOper: houghCircleDetect error: {}",e.what());
                         load.setBody({
                             {"error",e.what()}
                         });
                     }
                     this->network.get().sendLoad(load,response_addr);
                     return 0;
               }
   );
}
