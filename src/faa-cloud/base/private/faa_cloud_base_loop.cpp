//
// Created by HP on 2026/10/26.
//
#include "faa_cloud/microservices/base/service/private/faa_cloud_base_network_oper.h"

void faa::cloud::microservices::base::service::netWorkLoop(
        resource::OperatMap<net_work::FaaCloudNetworkRequest&>& operat_map,
        const resource::Network& network,
        const std::atomic<bool>& isRunning,
        core::thread_pool::FAAThreadPool& thread_pool,
        bool isRouter ,
        const std::function<void(net_work::FaaCloudNetworkRequest&)>& route_call,
        info::log::Ilog* log)
{
    std::vector<char> buffer;
    while (isRunning.load()) {

       buffer.resize(65536);
       auto request = net_work::FaaCloudNetworkRequest();
       network.get().getMessage(buffer, request);
       if (request.load.getHead().operation.empty()) {
            continue;
       }
       if (log) log->debug("netWorkLoop request: {}", request.serializeHead().dump());

       thread_pool.enqueue(
           [&operat_map, request = std::move(request), &network, log, route_call, isRouter]()

       mutable {
            try {

                auto head = request.load.getHead();

                   if (!head.router_to.empty() && isRouter) {
                       if (route_call) {
                           route_call(request);
                       }
                       return;
                   }

                   if (auto oper = operat_map.map.find(head.operation);
                       oper != operat_map.map.end()) {

                       if (log) log->debug("exec to : {}", oper->first);
                       oper->second(request);

                   }
                   else {
                        auto error_data = nlohmann::json::object();
                        error_data["body"]["error"] =
                                    "operation not found to " + head.operation;
                        request.clearBody();
                        request.deserializeBody(error_data);
                        network.get().sendMessage(request);
                    }

            } catch (std::exception& e) {
                if (log) log->error("netWorkLoop error: {}", e.what());
            }
       });
    }
}


