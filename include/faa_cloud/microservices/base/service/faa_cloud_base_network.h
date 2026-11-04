//
// Created by HP on 2026/10/12.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CLOUD_BASE_NETWORK_H
#define FASTAIASSISTANT_ROOT_FAA_CLOUD_BASE_NETWORK_H
#include "faa_cloud/microservices/base/service/private/faa_cloud_base_network_oper.h"
namespace  faa::cloud::microservices::base {
    template<typename T>
    T& FaaCloudBaseServices<T>::registerDefine(info::log::Ilog* log) {

        if (!service::registerToDefine(config, network, log)) {
            if (log) log->error("register failed");
        }
        return static_cast<T&>(*this);
    }
}
#endif //FASTAIASSISTANT_ROOT_FAA_CLOUD_BASE_NETWORK_H
