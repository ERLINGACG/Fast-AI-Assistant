//
// Created by HP on 2026/8/5.
//
#include "faa_service/service_event/faa_service_event.h"

#include <fstream>

#include "faa_service/service_event/operation/faa_service_udp_operation.h"

using faa::service::event::FaaServiceEventOrchestrator;

FaaServiceEventOrchestrator& FaaServiceEventOrchestrator::initNetwork(std::unique_ptr<net_work::FaaServiceUDPNetwork> network){
    this->network=std::move(network);
    return *this;
}

FaaServiceEventOrchestrator& FaaServiceEventOrchestrator::initMetaResources(const std::string& path){
    std::ifstream file(path);
    if(!file){
        this->ilog.error("init resource config failed, config path: {}",path);
        return *this;
    }
    file >> this->meta_resources_config_json;

    this->ilog.debug("init resource config success, config path: {}",path);
    return *this;
}

FaaServiceEventOrchestrator& FaaServiceEventOrchestrator::initThreadPools()
{
    this->resources.thread_pools.short_task_pool_size=10;
    this->resources.thread_pools.long_task_pool_size =10;
    if (!this->resources.thread_pools.init()){
        this->ilog.error("init thread pools failed");
    }else{
        this->ilog.debug("init thread pools success");
    }
    return *this;
}

FaaServiceEventOrchestrator& FaaServiceEventOrchestrator::start(){
    // resources.init();
    this->resources.threads.udp_thread=std::make_unique<std::thread>([this](){
        operation::udp_loop(*this);
    });
    this->resources.threads.cmd_thread=std::make_unique<std::thread>([this](){
        while (this->isRunning()){
            std::string line;
            std::getline(std::cin,line);
            if (line=="/exit"){
                this->stop();
                break;
            }

        }
    });
    return *this;
}

FaaServiceEventOrchestrator& FaaServiceEventOrchestrator::stop(){
    this->ilog.info("stop service");
    this->is_running.store(false);
    this->network->stop();
    return *this;
}

FaaServiceEventOrchestrator& FaaServiceEventOrchestrator::join(){
    this->resources.threads.udp_thread->join();
    this->resources.threads.cmd_thread->join();
    return *this;
}
