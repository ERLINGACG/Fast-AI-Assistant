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



FaaCloudRouter& FaaCloudRouter::initOperatorHandlerV2() {
    this->operator_map_v2["responseV2"]=[this](net_work::FaaCloudNetworkLoad& load, sockaddr_in& addr){
        return this->responseV2(load,addr);
    };
    this->operator_map_v2["register"]=[this](net_work::FaaCloudNetworkLoad& load, sockaddr_in& addr){
        return this->registerMsV2(load,addr);
    };
    this->operator_map_v2["query"]=[this](net_work::FaaCloudNetworkLoad& load, sockaddr_in& addr){
        return this->queryMsInfoV2(load,addr);
    };
    this->operator_map_v2["route"]=[this](net_work::FaaCloudNetworkLoad& load, sockaddr_in& addr){
        return this->routeMsInfoV2(load,addr);
    };
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

void FaaCloudRouter::loop(info::log::Ilog& logger)  {


    using faa::cloud::net_work::FaaCloudNetworkLoad;

    std::vector<char> buffer(65536);
    while (this->running){

        buffer.assign(buffer.size(), 0);
        std::string message;
        sockaddr_in client_addr{};
        if (!this->network || !this->thread_pool_){
            logger.error("network or thread pool is null");
            break;
        }

        this->network->getMessage(buffer,message,client_addr);
        logger.debug("get message: {}",message);

        this->thread_pool_->enqueue([this,message,client_addr,&logger](){

            char ip_str[INET_ADDRSTRLEN];
            const std::string ip(inet_ntop(AF_INET,&(client_addr.sin_addr), ip_str,INET_ADDRSTRLEN));

                    try{
                        if (message.empty()) {
                            return;
                        }
                        nlohmann::json requires_info=nlohmann::json::parse(message);
                           
                        FaaCloudNetworkLoad load;
                        load.serialize(client_addr,requires_info);

                        auto operator_func=this->operator_map_v2.find(
                            load.head().operation
                        );
                        if (!load.head().router_to.empty()){
                            operator_func=this->operator_map_v2.find(
                                "route"
                            );
                        }

                        if (operator_func!=this->operator_map_v2.end()){
                            operator_func->second(load,const_cast<sockaddr_in&>(client_addr));
                        }else{
                            logger.error("unknown operation: {}",requires_info.value("operation",""));
                            load.setBody({{"error","unknown operation"}});
                            this->network->sendLoad(
                                load,
                                   client_addr
                            );
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
