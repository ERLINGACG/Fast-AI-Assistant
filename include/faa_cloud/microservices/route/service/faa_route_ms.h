//
// Created by HP on 2026/10/24.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_ROUTE_MS_H
#define FASTAIASSISTANT_ROOT_FAA_ROUTE_MS_H
#include "faa_core/faa_export.h"
#include "faa_cloud/microservices/base/service/faa_cloud_base_services.h"
#include "faa_cloud/microservices/route/service/faa_route_ms_data.h"
namespace faa::cloud::microservices::route {
    class FAA_CPP_EXPORT FaaCloudRouteMS : public base::FaaCloudBaseServices<FaaCloudRouteMS> {

        service::RegisterMsSet set {};
        info::log::Ilog* log = nullptr;
        public:
            void setLog(info::log::Ilog* log);
            void addAllThread();
            void addNetWorkLoopThread();


            void addAllOper();
            void addRegisterOper();
            void addRouteOper();
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_ROUTE_MS_H