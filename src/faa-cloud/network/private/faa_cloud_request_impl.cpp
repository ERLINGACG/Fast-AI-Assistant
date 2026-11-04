//
// Created by HP on 2026/10/22.
//


#include "faa_cloud/network/faa_cloud_network_request.h"
#include "faa_cloud/network/private/faa_cloud_network_request_impl.h"
#include <ws2ipdef.h>
#include <ws2tcpip.h>
using faa::cloud::net_work::FaaCloudNetworkRequestImpl;
using faa::cloud::net_work::FaaCloudNetworkRequest;

FaaCloudNetworkRequest::FaaCloudNetworkRequest()
{
    impl_prt = std::make_unique<FaaCloudNetworkRequestImpl>();
}

FaaCloudNetworkRequest::~FaaCloudNetworkRequest() = default;
FaaCloudNetworkRequest::FaaCloudNetworkRequest(FaaCloudNetworkRequest&&) noexcept = default;
FaaCloudNetworkRequest& FaaCloudNetworkRequest::operator=(FaaCloudNetworkRequest&&) noexcept = default;

void FaaCloudNetworkRequest::resetNetwork(const std::string& ip, const uint16_t port) const {
   auto& client_addr = this->impl()->client_addr;
    ZeroMemory(&client_addr, sizeof(client_addr));  // 必须清零
    inet_pton(AF_INET, ip.c_str(), &client_addr.sin_addr);   // 设置目标 IP
    client_addr.sin_family = AF_INET;
    client_addr.sin_port   = htons(port);             // 目标端口
}

void FaaCloudNetworkRequest::clear() {

}

void FaaCloudNetworkRequest::deserialize(const nlohmann::json& data) {
    this->deserializeHead(data);
    this->deserializeBody(data);
}

void FaaCloudNetworkRequest::deserializeHead(const nlohmann::json& data) {
    this->load.head.deserialize(data);
}


void FaaCloudNetworkRequest::deserializeBody(const nlohmann::json& data) {
    this->load.body.deserialize(data);
}

void FaaCloudNetworkRequest::clearBody()
{
    this->load.body.clear();
}

nlohmann::json FaaCloudNetworkRequest::serializeHead() const
{
    return nlohmann::json::object({
        {"operation",this->load.head.operation},
        {"router_to",  this->load.head.router_to},
        {"request_ip", this->load.head.request_ip},
        {"request_port",this->load.head.request_port},
        {"slice_len",    this->load.head.slice_len},
        {"br_host",      this->load.head.br_host},
        {"br_port",      this->load.head.br_port}
    });
}

nlohmann::json FaaCloudNetworkRequest::serializeBody() const {
    return load.body.serialize();
}

nlohmann::json FaaCloudNetworkRequest::serialize() const {

    nlohmann::json json     = this->serializeBody();
    json["operation"]    = this->load.head.operation;
    json["router_to"]    = this->load.head.router_to;
    json["request_ip"]   = this->load.head.request_ip;      // ← 改成从 head 取
    json["request_port"] = this->load.head.request_port;    // ← 改成从 head 取
    json["slice_len"]    = this->load.head.slice_len;
    json["br_host"]      = this->load.head.br_host;
    json["br_port"]      = this->load.head.br_port;

    return json;
}

void FaaCloudNetworkRequest::setBody(const nlohmann::json& data)
{
    this->load.setBody(data);
}
