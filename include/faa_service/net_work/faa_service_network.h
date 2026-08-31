//
// Created by HP on 2026/7/25.
//

#ifndef FASTSCREEN_FAA_SERVICE_NETWORK_H
#define FASTSCREEN_FAA_SERVICE_NETWORK_H
#include "faa_net_work.h"
#include "info/ilog/ilog.h"
#include "faa-core/thread_pool/faa_thread_pool.h"
namespace faa::service::net_work{

      class FaaServiceUDPNetwork{
            NetWorkConfig iconfig{};
            NetWorkUDPRuntime runtime{};
            info::log::Ilog ilog;

            public:
                FaaServiceUDPNetwork(const info::log::Ilog ilog):ilog(ilog){}
                FaaServiceUDPNetwork& setConfig(const NetWorkConfig& config);
                FaaServiceUDPNetwork& init();


                void stop();
                [[nodiscard]] auto log() const{return ilog;}
                [[nodiscard]] auto config() const{return iconfig;}



                void getMessage(std::vector<char>& buffer,std::string& message,sockaddr_in& client_addr) const;
                void sendMessage(const std::string& message,const sockaddr_in& client_addr) const;


      };


}
#endif //FASTSCREEN_FAA_SERVICE_NETWORK_H