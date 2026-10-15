#include <thread>

#include "faa_core/thread_pool/faa_thread_pool.h"
#include "faa_service/net_work/faa_service_network.h"

using faa::service::net_work::FaaServiceUDPNetwork;

FaaServiceUDPNetwork& FaaServiceUDPNetwork::setConfig(const NetWorkConfig& config){
    this->iconfig=config;
    this->ilog.debug("setConfig: ip:{} max_threads:{} buffer_size: {}",
        config.ip.c_str(),
        config.max_threads,
        config.buffer_size
    );

    return *this;
}

FaaServiceUDPNetwork& FaaServiceUDPNetwork::init(){
    auto wsa_startup = [&](){
        if (WSAStartup(MAKEWORD(2, 2), &this->runtime.wsaData) != 0) {
            this->log().error("WSAStartup failed: {}", WSAGetLastError());
            return false;
        }
        this->log().info("WSAStartup success");
        return true;
    };
    auto wsa_create_socket = [&]() {
        this->runtime.server_socket= socket(AF_INET, SOCK_DGRAM, 0);
        if (this->runtime.server_socket == INVALID_SOCKET) {
            this->log().error("socket create failed: {}", WSAGetLastError());
            WSACleanup();
            return false;
        }
        this->log().info("socket create success");
        return true;
    };
    auto bind_to_socket = [&]() {

        this->runtime.server_addr.sin_family = AF_INET; // IPv4
        this->runtime.server_addr.sin_addr.s_addr = htonl(INADDR_ANY);          // 监听所有本地 IP[reference:13]
        this->runtime.server_addr.sin_port = htons(this->config().start_port);   // 设置端口[reference:14]

        if (bind(this->runtime.server_socket,
          reinterpret_cast<struct sockaddr*>(&this->runtime.server_addr),
          sizeof(this->runtime.server_addr)) == SOCKET_ERROR) {
            // 处理错误
            this->log().error("bind failed: {}", WSAGetLastError());
            closesocket(runtime.server_socket); // 关闭套接字
            WSACleanup();
            return false;
          }

        this->log().info("bind success");
        return true;
    };
    if (!wsa_startup() || !wsa_create_socket() || !bind_to_socket()) {
        this->log().error("wsa_startup or create_socket or bind_to_socket failed");
        return *this;
    }
    return *this;
}




void FaaServiceUDPNetwork::stop() {
    if (runtime.server_socket != INVALID_SOCKET) {
        closesocket(runtime.server_socket);
        runtime.server_socket = INVALID_SOCKET;
        WSACleanup();
        this->log().info("server socket closed");
    }
}

void FaaServiceUDPNetwork::getMessage(std::vector<char>& buffer,std::string& message,sockaddr_in& client_addr) const {
    int client_addr_len = sizeof(client_addr);
    int bytes_received = recvfrom(
                  this->runtime.server_socket,
                  buffer.data(),
                  static_cast<int>(buffer.size()) - 1,
                  0,
                  reinterpret_cast<struct sockaddr*>(&client_addr),
                  &client_addr_len);
    if (bytes_received == SOCKET_ERROR) {
        const int error = WSAGetLastError();

        if (error == WSAECONNRESET) return;
        if (error == WSAENOTSOCK || error == WSAEINTR) {
            this->log().info("the loop is exit");
            return;
        }
        this->log().error("recvfrom failed: {}", error);
        return;
    }
    message=std::string(
        buffer.data(),
        bytes_received
    );
}

void FaaServiceUDPNetwork::sendMessage(const std::string& message,const sockaddr_in& client_addr) const {
    sendto(
        this->runtime.server_socket,
        message.c_str(),
        static_cast<int>(message.size()),
        0,
        reinterpret_cast<const sockaddr*>(&client_addr),
        sizeof(client_addr)
    );
}
