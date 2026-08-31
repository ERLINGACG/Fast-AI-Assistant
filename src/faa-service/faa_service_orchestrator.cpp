//
// Created by HP on 2026/7/27.
//
#include "faa_service/faa_service_orchestrator.h"

#include <thread>

#include "faa-core/thread_pool/faa_thread_pool.h"
#include "faa_service/scheduler/faa_service_scheduler.h"
using faa::service::orchestrator::FaaServiceOrchestrator;


FaaServiceOrchestrator& FaaServiceOrchestrator::initResourceConfig(const std::string& config_path)
{
    std::ifstream file(config_path);
    if(!file){
        this->ilog.error("init resource config failed, config path: {}",config_path);
        return *this;
    }
    file >> this->resources_config_json;
    this->ocr_resources.set_config(
        this->resources_config_json["ocr_config"]
    );
    this->ilog.debug("init resource config success, config path: {}",config_path);
    return *this;
}

FaaServiceOrchestrator& FaaServiceOrchestrator::initNetworkToUdp(std::unique_ptr<net_work::FaaServiceUDPNetwork> network){

    this->network=std::move(network);
    this->network->log().debug("init network");
    return *this;
}

FaaServiceOrchestrator& FaaServiceOrchestrator::initThreadPools(int32_t max_threads)
{
    this->run_thread_pools_.ask_pool=std::make_unique<core::thread_pool::FAAThreadPool>(
        max_threads?max_threads:this->network->config().max_threads
    );
    this->run_thread_pools_.init_pool=std::make_unique<core::thread_pool::FAAThreadPool>(
        max_threads?max_threads:this->network->config().max_threads
    );
    this->run_thread_pools_.long_loop_pool=std::make_unique<core::thread_pool::FAAThreadPool>(
        max_threads?max_threads:this->network->config().max_threads
    );
    this->run_thread_pools_.proc_pool=std::make_unique<core::thread_pool::FAAThreadPool>(
        max_threads?max_threads:this->network->config().max_threads
    );

    return *this;
}

FaaServiceOrchestrator& FaaServiceOrchestrator::start()
{
    this->run_threads_.udp_thread=std::thread([this](){
        scheduler::loop_to_udp<FaaServiceOrchestrator>(*this);
    });
    this->run_threads_.cmd_thread=std::thread([this](){
        while(this->is_running.load()){
            std::string cmd;
            std::cin >> cmd;
            if(cmd=="/exit"){
                this->is_running.store(false);
                this->network->stop();
                break;
            }
        }
    });

    return *this;
}

FaaServiceOrchestrator& FaaServiceOrchestrator::join(){
    this->run_threads_.udp_thread.join();
    this->run_threads_.cmd_thread.join();
    return *this;
}

