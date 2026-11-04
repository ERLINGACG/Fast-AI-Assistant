//
// Created by HP on 2026/10/29.
//
#include "faa_cloud/microservices/base/service/private/faa_cloud_base_network_oper.h"
void faa::cloud::microservices::base::service::netWorkSend(
    const resource::Network& network,
    net_work::FaaCloudNetworkRequest& request,
    const std::function<void(net_work::FaaCloudNetworkRequest&)>& call,
    info::log::Ilog* log) {

    if (call) {
        try {
            call(request);
        } catch (const std::exception& e) {
            if (log) log->error("netWorkSend error: {}", e.what());
        }
    }
    if (const auto head = request.load.getHead(); head.br_port == 0 || head.br_host.empty()) {
        request.resetNetwork(
            head.request_ip,
            head.request_port
        );
    }else {
        request.resetNetwork(
            head.br_host,
            head.br_port
        );
    }
    network.get().sendMessage(request);
}

void faa::cloud::microservices::base::service::netWorkSendAsync(const resource::Network& network,
    net_work::FaaCloudNetworkRequest& request, const std::function<void(net_work::FaaCloudNetworkRequest&)>& call,
    info::log::Ilog* log)
{
    if (const auto head = request.load.getHead(); head.br_port == 0 || head.br_host.empty()) {
        request.resetNetwork(
            head.request_ip,
            head.request_port
        );
    }else {
        request.resetNetwork(
            head.br_host,
            head.br_port
        );
    }
    network.get().sendMessage(request);
    if (call) {
        try {
            call(request);
        } catch (const std::exception& e) {
            if (log) log->error("netWorkSendAsync error: {}", e.what());
        }
    }
}
