//
// Created by HP on 2026/8/5.
//

#ifndef FASTSCREEN_FAA_SERVICE_EVENT_H
#define FASTSCREEN_FAA_SERVICE_EVENT_H
#include "faa_service/net_work/faa_service_network.h"
#include "faa_service/resources/faa_service_resources.h"
#include "nlohmann/json.hpp"
namespace faa::service::event{



    class FaaServiceEventOrchestrator{

        std::atomic<bool> is_running{true};

        std::unique_ptr<net_work::FaaServiceUDPNetwork> network;
        resources::FaaServiceResources resources;


    protected:
        nlohmann::json    meta_resources_config_json;
        info::log::Ilog ilog;
    public:
       FaaServiceEventOrchestrator(const info::log::Ilog& ilog):ilog(ilog){}



       FaaServiceEventOrchestrator& initNetwork(std::unique_ptr<net_work::FaaServiceUDPNetwork> network);
       FaaServiceEventOrchestrator& initMetaResources(const std::string& path);
       FaaServiceEventOrchestrator& initThreadPools();
       FaaServiceEventOrchestrator& start();
       FaaServiceEventOrchestrator& stop();
       FaaServiceEventOrchestrator& join();


       std::atomic<bool>& isRunning(){
           return is_running;
       }
       auto& getLog(){
           return ilog;
       }
       auto& getNetwork() const {
           return *network;
       }

       auto& getResources(){
           return resources;
       }

       auto& getMetaConfig(){
           return meta_resources_config_json;
       }

    };
}
#endif //FASTSCREEN_FAA_SERVICE_EVENT_H