//
// Created by HP on 2026/10/27.
//
#include "faa_cloud/microservices/inputs/service/faa_inputs_ms.h"
using faa::cloud::microservices::inputs::FaaCloudInputMS;

void FaaCloudInputMS::setLog(info::log::Ilog* log) {
    this->log = log;
    this->keyboardInput.setLogger(log);
    this->mouseInput.setLogger(log);
}

void FaaCloudInputMS::addAllOper() {
    addHelloOperat();
    addSetTagOperat();
    addMouseApplyOperat();
    addActionOperat();
    addKeyboardApplyOperat();
}



void FaaCloudInputMS::addHelloOperat() {
    if (log) log->info("addHelloOperat");
    this->operat.push("hello",[this](net_work::FaaCloudNetworkRequest& request){

        if (log) log->info("hello");
        std::cout << "hello" << std::endl;

        auto hello_data = nlohmann::json::object();
        hello_data["body"]["hello"] = "hello";
        request.deserializeBody(hello_data);


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

        this->network.get().sendMessage(request);
        return 0;
    });
}

void FaaCloudInputMS::addSetTagOperat() {
    if (log) log->info("addSetTagOperat");
    this->operat.push("set_tag",[this](net_work::FaaCloudNetworkRequest& request){

        base::service::netWorkSend(
            this->network,request,
            [this](const net_work::FaaCloudNetworkRequest& request){
                         const auto tag_data = request.serializeBody().value(
                             "body",nlohmann::json::object()
                         );
                         try{
                              auto tag = tag_data.value("tag","");
                              this->keyboardInput.resetTarget(tag);
                              this->mouseInput.resetTarget(tag);

                          }catch(const std::exception& e){
                              log->error("reset window handle failed: {}", e.what());
                          }
            },
            this->log
        );

        return 0;
    });
}


