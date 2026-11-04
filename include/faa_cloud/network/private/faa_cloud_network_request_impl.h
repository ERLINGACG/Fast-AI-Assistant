//
// Created by HP on 2026/10/22.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CLOUD_NETWORK_REQUEST_IMPL_H
#define FASTAIASSISTANT_ROOT_FAA_CLOUD_NETWORK_REQUEST_IMPL_H
#include <winsock2.h>
namespace faa::cloud::net_work
{
    struct FaaCloudNetworkRequestImpl {
        sockaddr_in client_addr{};
        // sockaddr_in back_addr{};
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_CLOUD_NETWORK_REQUEST_IMPL_H