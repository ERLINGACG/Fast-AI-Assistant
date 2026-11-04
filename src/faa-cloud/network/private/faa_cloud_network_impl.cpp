//
// Created by HP on 2026/10/22.
//
#include "faa_cloud/network/private/faa_cloud_network_impl.h"
#include "faa_cloud/network/faa_cloud_network_ctx.h"
using faa::cloud::net_work::FaaCloudNetworkCtx;
using faa::cloud::net_work::FaaCloudNetworkCtxImpl;


FaaCloudNetworkCtx::FaaCloudNetworkCtx() {
    this->impl_prt = std::make_unique<FaaCloudNetworkCtxImpl>();
};
FaaCloudNetworkCtx::~FaaCloudNetworkCtx() = default;
FaaCloudNetworkCtx::FaaCloudNetworkCtx(FaaCloudNetworkCtx&&) noexcept = default;
FaaCloudNetworkCtx& FaaCloudNetworkCtx::operator=(FaaCloudNetworkCtx&&) noexcept = default;

bool FaaCloudNetworkCtxImpl::init(const nlohmann::json& config, info::log::Ilog* log) {

// #ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        if (log) log->error("WSAStartup failed: {}", WSAGetLastError());
        return false;
    }
    server_socket= socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == INVALID_SOCKET) {
        if (log) log->error("socket create failed: {}", WSAGetLastError());
        WSACleanup();
        return false;
    }
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);          // 监听所有本地 IP[reference:13]
    server_addr.sin_port = htons(config.value("port",8080));   // 设置端口[reference:14]

    if (bind(server_socket,
             reinterpret_cast<struct sockaddr*>(&server_addr),
             sizeof(server_addr)) == SOCKET_ERROR) {
        // 处理错误
        if (log) log->error("bind failed: {}", WSAGetLastError());
        closesocket(server_socket); // 关闭套接字
        WSACleanup();
        return false;
    }
    if (log) log->info("socket create success, port: {}",config.value("port",8080));


// #endif

    return true;
}

void FaaCloudNetworkCtxImpl::close()
{
    if (server_socket != INVALID_SOCKET)
    {
        closesocket(server_socket);
        server_socket = INVALID_SOCKET;
        WSACleanup();

    }
}


