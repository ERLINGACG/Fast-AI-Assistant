//
// Created by HP on 2026/8/25.
//
#include "faa_cloud/network/faa_cloud_network.h"

faa::cloud::net_work::FaaCloudNetwork& faa::cloud::net_work::FaaCloudNetwork::init(const nlohmann::json& config)
{
    this->ctx.config(config);
    this->ctx.init(this->log);
    return *this;
}

void faa::cloud::net_work::FaaCloudNetwork::close()
{
    if (ctx.server_socket != INVALID_SOCKET)
    {
        closesocket(ctx.server_socket);
        ctx.server_socket = INVALID_SOCKET;
        WSACleanup();
        log.info("close server socket");
    }
}

void faa::cloud::net_work::FaaCloudNetwork::getMessage(std::vector<char>& buffer, std::string& message,
                                                       sockaddr_in& client_addr)
{
    int client_addr_len = sizeof(client_addr);
    int bytes_received = recvfrom(
        this->ctx.server_socket,
        buffer.data(),
        static_cast<int>(buffer.size()) - 1,
        0,
        reinterpret_cast<struct sockaddr*>(&client_addr),
        &client_addr_len);
    if (bytes_received == SOCKET_ERROR)
    {
        const int error = WSAGetLastError();

        if (error == WSAECONNRESET) return;
        if (error == WSAENOTSOCK || error == WSAEINTR)
        {
            this->log.info("the loop is exit");
            return;
        }
        this->log.error("recvfrom failed: {}", error);
        return;
    }
    message = std::string(
        buffer.data(),
        bytes_received
    );
}

void faa::cloud::net_work::FaaCloudNetwork::sendMessage(const std::string& message,
                                                        const sockaddr_in& client_addr) const
{
    sendto(
        this->ctx.server_socket,
        message.c_str(),
        static_cast<int>(message.size()),
        0,
        reinterpret_cast<const sockaddr*>(&client_addr),
        sizeof(client_addr)
    );
}

void faa::cloud::net_work::FaaCloudNetwork::sendLoad(FaaCloudNetworkLoad& load,
                                                     const sockaddr_in& client_addr) const
{

    // load.serialize(client_addr);
    this->sendMessage(load.unserialize().dump(), client_addr);
    for (const auto& slice : load.body_.slices) {
        std::cout<<slice.dump()<<std::endl;
        this->sendMessage(slice.dump(), client_addr);
    }
}
