//
// Created by HP on 2026/8/25.
//
#include "faa_cloud/router/faa_cloud_router.h"
#include "faa_cloud/network/faa_cloud_network.h"
#include <ws2tcpip.h>

#include "faa_cloud/network/faa_cloud_network_ack.h"
using faa::cloud::router::FaaCloudRouter;

FaaCloudRouter& FaaCloudRouter::initThreadPool(){
    this->thread_pool_=std::make_unique<core::thread_pool::FAAThreadPool>();

    return *this;
}

FaaCloudRouter& FaaCloudRouter::initOperatorHandler(){

    auto handler_register=[this](nlohmann::json& requires_detail, sockaddr_in& addr){
        return this->registerMs(requires_detail,addr);
    };
    auto handler_query=[this](nlohmann::json& requires_detail, sockaddr_in& addr){
        return this->queryMsInfo(requires_detail,addr);
    };
    auto handler_route=[this](nlohmann::json& requires_detail, sockaddr_in& addr){
        return this->routeMsInfo(requires_detail,addr);
    };

    this->operator_map["register"]=handler_register;
    this->operator_map["query"]   =handler_query;
    this->operator_map["route"]   =handler_route;

    return *this;
}

FaaCloudRouter& FaaCloudRouter::start(info::log::Ilog& logger) {
    this->running=true;
    this->run_thread=std::thread([this,&logger](){
        this->loop(logger);
    });
    this->cmd_thread=std::thread([this,&logger](){
        this->cmd_loop(logger);
    });
    return *this;
}

FaaCloudRouter& FaaCloudRouter::join() {
    this->run_thread.join();
    this->cmd_thread.join();
    return *this;
}

bool FaaCloudRouter::registerMs(nlohmann::json& ms_info, sockaddr_in& addr){
    net_work::FaaCloudNetworkAck ack{
    };
    auto ack_json=nlohmann::json{};
    if (ms_info.contains("service_name")){

        this->register_mapper.registerMs(
            this->id.load(),
            ms_info["service_name"].get<std::string>()+"_"+std::to_string(this->id.load()),
            addr
            );

        ack_json["id_str"]=ms_info["service_name"].get<std::string>()+"_"+std::to_string(this->id.load());
        ack.unserialize("success",ack_json);
        this->network->sendMessage(
            ack.serialize(),
            addr
        );
        this->id.fetch_add(1);

        return true;
    }
    ack_json["error"]="service_name is not provided";
    ack.unserialize("register failed",ack_json);
    this->network->sendMessage(
        ack.serialize(),
        addr
    );


    std::vector<char> buffer;
    std::string cline_ack_str;
    this->network->getMessage(buffer,cline_ack_str,addr);
    auto logger=this->network->getLog();
    logger.info("get ack: {}",cline_ack_str);

    try{
        ack_json=nlohmann::json::parse(cline_ack_str);

        if (ack_json.contains("info")){

            if (const std::string isOK=ack_json["info"].value("ack","");
                isOK!="ok"){

                logger.error("register failed: {}",ack_json["info"].value("error",""));
                this->register_mapper.unregisterMs(this->id.load());
                return false;

            }

        }
    }  catch(std::exception& e){
        logger.error("parse ack failed: {}",e.what());
    }
    return false;
}

bool FaaCloudRouter::queryMsInfo(nlohmann::json& ms_info, sockaddr_in& addr){
    net_work::FaaCloudNetworkAck ack{
    };
    ack.unserialize("success",this->register_mapper.getInfo());
    this->network->sendMessage(
        ack.serialize(),
        addr
    );
    return true;
}

bool FaaCloudRouter::routeMsInfo(nlohmann::json& requires_json, sockaddr_in& addr){

    if (requires_json.contains("name_str")){
        const auto ms=this->register_mapper.getMsInfo(requires_json["name_str"].get<std::string>());
        auto ack=net_work::FaaCloudNetworkAck();
        auto send_client_json=nlohmann::json();

        if (ms.name.empty()){
            send_client_json["error"]="ms not found";
            ack.unserialize("register failed",send_client_json);
            this->network->sendMessage(
                ack.serialize(),
                addr
            );
            return false;
        }

        char ip_str[INET_ADDRSTRLEN];
        const std::string requester_ip(inet_ntop(AF_INET,&(addr.sin_addr), ip_str,INET_ADDRSTRLEN));
        const uint16_t requester_port = ntohs(addr.sin_port);


        send_client_json["requires_ip"]      =requester_ip;
        send_client_json["requires_port"]   =requester_port;
        send_client_json["requires_detail"]    =requires_json.value("detail",nlohmann::json());

        ack.unserialize("success",send_client_json);

        this->network->sendMessage(
            ack.serialize(),
            ms.addr
        );
        return  true;
    }

    return false;
}

void FaaCloudRouter::loop(info::log::Ilog& logger)  {

    while (this->running){
        std::vector<char> buffer(65536);
        std::string message;
        sockaddr_in client_addr{};
        if (!this->network || !this->thread_pool_){
            logger.error("network or thread pool is null");
            break;
        }

        this->network->getMessage(buffer,message,client_addr);

        this->thread_pool_->enqueue([this,message,client_addr,&logger](){

            char ip_str[INET_ADDRSTRLEN];
            const std::string ip(inet_ntop(AF_INET,&(client_addr.sin_addr), ip_str,INET_ADDRSTRLEN));
                    if (!message.empty()) {
                        logger.debug("get message: {} from {}",message, ip);
                    }else{
                        return;
                    }
                    try{

                        nlohmann::json requires_info=nlohmann::json::parse(message);

                        if (!requires_info.contains("route_operation")){
                            return;
                        }
                        auto ack =net_work::FaaCloudNetworkAck();
                        const auto operator_func=this->operator_map.find(requires_info.value("route_operation",""));

                        if (operator_func!=this->operator_map.end()){
                            operator_func->second(requires_info,const_cast<sockaddr_in&>(client_addr));
                        }else{

                            ack.unserialize(
                                "unknown operation",
                                nlohmann::json(
                                    {"operation",requires_info.value("route_operation","")}
                                )
                            );
                            this->network->sendMessage(ack.serialize(),client_addr);
                            logger.error("unknown operation: {}",requires_info.value("route_operation",""));
                        }

                    }catch(std::exception& e){
                        logger.error("parse message failed: {}",e.what());
                    }
                    std::cout<<std::endl;

            }
        );
    }
}

void FaaCloudRouter::cmd_loop(info::log::Ilog& logger)
{
    while (this->running){
        std::string cmd;
        std::cin>>cmd;
        if (cmd=="/exit"){
            this->running.store(false);
            this->network->close();
            break;
        }
    }

}
