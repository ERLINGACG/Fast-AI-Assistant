//
// Created by HP on 2026/8/5.
//

#ifndef FASTSCREEN_FAA_SERVICE_UDP_OPERATION_H
#define FASTSCREEN_FAA_SERVICE_UDP_OPERATION_H
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
// #include <ws2def.h>

#include "faa_service_task_operation.h"
#include "../faa_service_event_ack.h"
#include "faa_service/service_event/faa_service_event_task.h"

namespace faa::service::event::operation{

    template<typename T>
    inline void udp_loop(T& t);


}

template <typename T>
void faa::service::event::operation::udp_loop(T& t){
    auto& network=t.getNetwork();
    auto log =t.getLog();
    std::vector<char> buffer(network.config().buffer_size);

    while(t.isRunning()){
        sockaddr_in client_addr{};
        std::string message;
        network.getMessage(buffer,message,client_addr);
        //
        log.info("Received message: {}",message);
        try
        {
            auto task=Task();
            if(!message.empty()){
                task.buildTask(nlohmann::json::parse(message));
                service::operation::submit_long_task(t,task);
                service::operation::submit_short_task(t,task,client_addr);
            }

        }catch (std::exception& e){
                log.error("Error: {}",e.what());
        }

        // std::cout<<std::endl;
        // auto ack=Ack{
        //     0,
        //     "success",
        //     ""
        // };
        // ack.detail();
        // network.sendMessage(ack.toString(),client_addr);
    }
}

#endif //FASTSCREEN_FAA_SERVICE_UDP_OPERATION_H