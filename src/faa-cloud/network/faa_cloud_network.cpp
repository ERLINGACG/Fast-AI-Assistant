//
// Created by HP on 2026/8/25.
//
#include "faa_cloud/network/faa_cloud_network.h"



#include "faa_cloud/network/private/faa_cloud_network_impl.h"
#include "faa_cloud/network/private/faa_cloud_network_request_impl.h"
#include "faa_cloud/network/faa_cloud_network_load.h"

#include <ws2ipdef.h>
#include <ws2tcpip.h>

using faa::cloud::net_work::FaaCloudNetworkRequestImpl;
using faa::cloud::net_work::FaaCloudNetworkCtxImpl;
using faa::cloud::net_work::FaaCloudNetworkLoad;

faa::cloud::net_work::FaaCloudNetwork::FaaCloudNetwork(info::log::Ilog* log)
{
    this->log = log;
}

faa::cloud::net_work::FaaCloudNetwork& faa::cloud::net_work::FaaCloudNetwork::init(const nlohmann::json& config)
{
    this->ctx.config(config);
    this->ctx.impl().init(
        this->ctx.config_,
        this->log
    );

    return *this;
}

void faa::cloud::net_work::FaaCloudNetwork::close() const {

    this->ctx.impl().close();
}



void faa::cloud::net_work::FaaCloudNetwork::getMessage(
    std::vector<char>& buffer,
    FaaCloudNetworkRequest& request) const
{


    int client_addr_len = sizeof(request.impl()->client_addr);
    int bytes_received = recvfrom(
        this->ctx.impl().server_socket,
        buffer.data(),
        static_cast<int>(buffer.size()) - 1,
        0,
        reinterpret_cast<struct sockaddr*>(&request.impl()->client_addr),
        &client_addr_len);

    sockaddr_in src{};
    std::memcpy(&src, &request.impl()->client_addr, sizeof(src));

    char ip[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &src.sin_addr, ip, sizeof(ip));
    const auto src_port = ntohs(src.sin_port);


    if (bytes_received == SOCKET_ERROR) {
        const int error = WSAGetLastError();
        if (error == WSAEWOULDBLOCK || error == WSAETIMEDOUT) {
            return;
        }
        if (error == WSAECONNRESET) return;
        if (error == WSAENOTSOCK || error == WSAEINTR) {
            this->log->info("the loop is exit");
            return;
        }
        this->log->error("recvfrom failed: {}", error);
        return;
    }
    auto message = std::string(
        buffer.data(),
        bytes_received
    );
    try {
        auto json = nlohmann::json::parse(message);
        json["request_ip"]  = ip;
        json["request_port"]= src_port;

        const auto back_request_host = json.value("br_host","");
        const auto back_request_port    = json.value("br_port",0);
        const auto route_to = json.value("router_to","");

        if  (!route_to.empty() &&
            (back_request_host.empty() && back_request_port == 0)
        )  {
            json["br_host"]  = ip;
            json["br_port"]  = src_port;
        }

        request.deserialize (json);

    } catch (const std::exception& e) {
        this->log->error("parse message failed: {}", e.what());
    }
}

void faa::cloud::net_work::FaaCloudNetwork::sendMessage(const FaaCloudNetworkRequest& request) const {


    auto message = request.serialize().dump();
    sendto(
        this->ctx.impl().server_socket,
        message.c_str(),
        static_cast<int>(message.size()),
        0,
        reinterpret_cast<const sockaddr*>(&request.impl()->client_addr),
        sizeof(request.impl()->client_addr)
    );

    // auto slices = nlohmann::json::object();
    // int count = 0;
    // for (const auto& slice : request.load.body.slices) {
    //     const auto key = "slice" + std::to_string(count);
    //     slices[key] = slice;
    //     ++count;
    //
    //     slices.clear();
    // }
}
