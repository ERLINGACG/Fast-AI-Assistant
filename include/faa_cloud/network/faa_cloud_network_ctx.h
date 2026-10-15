//
// Created by HP on 2026/8/24.
//

#ifndef FASTSCREEN_FAA_CLOUD_NETWORK_CTX_H
#define FASTSCREEN_FAA_CLOUD_NETWORK_CTX_H
#include <winsock2.h>
#include <nlohmann/json.hpp>

#include "info/ilog/ilog.h"
#include "../code/faa_cloud_code.h"

namespace faa::cloud::net_work {

    struct FaaCloudNetworkCtx{
        nlohmann::json config_;

        WSADATA wsaData{};
        SOCKET server_socket{};
        sockaddr_in server_addr{};

        auto config(const nlohmann::json& config){
            config_=config;
            return code::success;
        }

        auto init(info::log::Ilog& log) {

            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                log.error("WSAStartup failed: {}", WSAGetLastError());
                return code::network_error;
            }

            server_socket= socket(AF_INET, SOCK_DGRAM, 0);
            if (server_socket == INVALID_SOCKET) {
                log.error("socket create failed: {}", WSAGetLastError());
                WSACleanup();
                return code::network_error;
            }
            server_addr.sin_family = AF_INET; // IPv4
            server_addr.sin_addr.s_addr = htonl(INADDR_ANY);          // 监听所有本地 IP[reference:13]
            server_addr.sin_port = htons(this->config_.value("port",8080));   // 设置端口[reference:14]

            if (bind(server_socket,
              reinterpret_cast<struct sockaddr*>(&server_addr),
              sizeof(server_addr)) == SOCKET_ERROR) {
                // 处理错误
                log.error("bind failed: {}", WSAGetLastError());
                closesocket(server_socket); // 关闭套接字
                WSACleanup();
                return code::network_error;
              }
            log.info("socket create success, port: {}",this->config_.value("port",8080));
            return code::success;
        };
    };
}
#endif //FASTSCREEN_FAA_CLOUD_NETWORK_CTX_H