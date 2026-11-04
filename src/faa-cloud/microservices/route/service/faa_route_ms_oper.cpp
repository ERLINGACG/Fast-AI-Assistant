//
// Created by HP on 2026/10/25.
//
#include "faa_cloud/microservices/route/service/faa_route_ms.h"
using faa::cloud::microservices::route::FaaCloudRouteMS;
using faa::cloud::net_work::FaaCloudNetworkRequest;
void FaaCloudRouteMS::addAllOper() {
    addRegisterOper();
    addRouteOper();
}

void FaaCloudRouteMS::addRegisterOper() {
    if (log) log->info("addRegisterOper");

    this->operat.push("register",[this](FaaCloudNetworkRequest& request){

       auto data = request.serialize();
       auto head   = request.load.getHead();
       auto body   = request.load.getBody();
       auto ms_info = service::RegisterMsInfo{
               .identity =    body.data.value("this_name",""),
               .host     = head.request_ip,
               .port     =    head.request_port,
               .timestamp=    std::chrono::system_clock::now().time_since_epoch().count(),
               .ttl_time =    LONG_MAX,
               .type     =    body.data.value("type",""),

       };
       this->set.push(ms_info,log);
       request.clearBody();
       auto response_body        = nlohmann::json::object();
       response_body["body"]["identity"] = ms_info.identity;

       request.deserializeBody(response_body);
       request.resetNetwork(
           request.load.getHead().request_ip,
           request.load.getHead().request_port
       );



       this->network.get().sendMessage(request);

       return 0;
    });
}

void FaaCloudRouteMS::addRouteOper() {
    if (log) log->info("addRouteOper");
    this->operat.push("route",[this](FaaCloudNetworkRequest& request){

        auto data = request.serialize();
        auto head   = request.load.getHead();


        auto ms_info = service::RegisterMsInfo{};
        if (!this->set.query(head.router_to,ms_info)){
            if (log) log->warn("[route] not found ms info: {}", head.router_to);

            request.clearBody();
            auto response_body        = nlohmann::json::object();
            response_body["body"]["error"]    = "not found ms";
            request.deserializeBody(response_body);

            request.resetNetwork(
                request.load.getHead().request_ip,
                request.load.getHead().request_port
            );

            this->network.get().sendMessage(request);
            return 0;
        }

        if (log) log->debug("[route] found ms info: {}", ms_info.identity);
        request.resetNetwork(
            ms_info.host,
            ms_info.port
        );
        this->network.get().sendMessage(request);


        return 0;
    });
}
