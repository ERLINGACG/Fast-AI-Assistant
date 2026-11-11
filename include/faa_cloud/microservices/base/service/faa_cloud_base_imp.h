//
// Created by HP on 2026/10/9.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CLOUD_BASE_IMP_H
#define FASTAIASSISTANT_ROOT_FAA_CLOUD_BASE_IMP_H
#include "info/ilog/ilog.h"

namespace faa::cloud::microservices::base
{
    template <typename T>
    T& FaaCloudBaseServices<T>::setConfig(nlohmann::json& config,info::log::Ilog* log) {
        if(log) {
            log->debug("set config {}",config.dump());
            this->config = config;
        }
        return static_cast<T&>(*this);;
    }



    template <typename T>
    T& FaaCloudBaseServices<T>::setNetwork(std::unique_ptr<net_work::FaaCloudNetwork> network,info::log::Ilog* log) {
        this->network.network=std::move(network);
        this->network.network->init(this->config);
        if(log) {
            log->debug("set network");

        }
        return static_cast<T&>(*this);;
    }

    template <typename T>
    T& FaaCloudBaseServices<T>::setSignal(const std::string& name,bool value) {
        this->signals.set(name,value);
        return static_cast<T&>(*this);
    }

    template <typename T>
    T& FaaCloudBaseServices<T>::start(info::log::Ilog* log) {
        using resource::ThreadsMap;

        for (auto& thread : this->threads.threads) {
            if(thread.second.status == ThreadsMap::running_status::ready){
                if(log) {
                    log->debug("start thread {}",thread.first);
                }

                thread.second.status = ThreadsMap::running_status::running;
                if(thread.second.type == ThreadsMap::running_type::join){

                    thread.second.thread.join();

                }else if(thread.second.type == ThreadsMap::running_type::detach){
                    thread.second.thread.detach();
                }
            }
        }
        return static_cast<T&>(*this);
    }

    template <typename T>
    T& FaaCloudBaseServices<T>::stop() {
        return static_cast<T&>(*this);
    }

    template <typename T>
    T& FaaCloudBaseServices<T>::init() {
        this->signals.set(isRunning,true);
        return static_cast<T&>(*this);
    }

    template <typename T>
    T& FaaCloudBaseServices<T>::close() {

        this->signals.set(isRunning,   false);
        return static_cast<T&>(*this);
    }

    template <typename T>
    T& FaaCloudBaseServices<T>::addThread(
        const std::string& name,
        std::thread thread,resource::ThreadsMap::running_type type,
        info::log::Ilog* log
        ) {

        this->threads.threads[name].thread = std::move(thread);
        this->threads.threads[name].type = type;

        if(log) {
            log->debug("add thread {} to map",name);
        }

        return static_cast<T&>(*this);
    }

    template <typename T>
    T& FaaCloudBaseServices<T>::addCommand(std::unique_ptr<command::FaaCommandTreeNode> cmd,
        const std::vector<std::string>& path, info::log::Ilog* log)
    {
        this->cmd.add(std::move(cmd),path,log);
        return static_cast<T&>(*this);
    }
}
#endif //FASTAIASSISTANT_ROOT_FAA_CLOUD_BASE_IMP_H
