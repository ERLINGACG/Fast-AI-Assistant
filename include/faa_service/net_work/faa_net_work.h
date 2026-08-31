//
// Created by HP on 2026/6/29.
//

#ifndef FASTSCREEN_FS_NET_WORK_H
#define FASTSCREEN_FS_NET_WORK_H
#include <cstdint>
#include <string>
#include <vector>
#include <winsock2.h>

namespace faa::service::net_work {

    struct NetWorkConfig {
        std::string ip;
        int32_t start_port;
        int32_t buffer_size=1024;
        int32_t max_threads=4;
    };

    struct NetWorkUDPRuntime {
        WSADATA wsaData{};
        SOCKET server_socket{};
        sockaddr_in server_addr{};

    };


}
#endif //FASTSCREEN_FS_NET_WORK_H