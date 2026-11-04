//
// Created by HP on 2026/10/12.
//
#include "faa_cloud/microservices/general/service/faa_cloud_general.h"
using faa::cloud::microservices::general::FaaCloudGeneralServices;


void FaaCloudGeneralServices::addNetworkThread(info::log::Ilog* log) {
    using net_work::FaaCloudNetworkLoad;

    log->debug("addNetworkThread");
    std::cout<<std::endl;
    this->addThread("networkThread", std::thread([this,log]() {

                        std::vector<char> buffer(65535);
                        sockaddr_in target_addr{};
                        while (this->getSignalMap().get(this->isRunning)) {

                            buffer.assign(buffer.size(), 0);
                            std::string message;

                            this->network.get().getMessage(buffer,message,target_addr);
                            if (message.empty()){
                                continue;
                            }
                            log->debug("message : {}",message);

                            FaaCloudNetworkLoad load;
                            load.deserialize(nlohmann::json::parse(message));

                            sockaddr_in response_addr{};
                            ZeroMemory(&response_addr, sizeof(response_addr));  // 必须清零
                            if (!load.head().request_ip.empty()){
                                response_addr.sin_family = AF_INET;
                                response_addr.sin_port = htons(load.head().request_port);             // 目标端口
                                inet_pton(AF_INET, load.head().request_ip.c_str(), &response_addr.sin_addr);
                            }else{
                                response_addr=target_addr;
                            }

                            //
                            this->thread_pool.enqueue([this,message,load,response_addr,log]() mutable
                                {
                                    try {

                                        log->debug("load.head().operation : {}",load.head().operation);
                                        auto op = this->operat.map.find(load.head().operation);
                                        if (op!=this->operat.map.end()){
                                            op->second(load,response_addr);
                                        }else{
                                            load.setBody({{"error","unknown operation"}});
                                            this->network.get().sendLoad(
                                                load,
                                                response_addr
                                            );
                                        }
                                        std::cout<<std::endl;

                                    } catch (std::exception& e){
                                        log->error("addNetworkThread error : {}",e.what());

                                    }
                                }
                            );

                        }
                    }),
                    base::resource::ThreadsMap::join,log
    );
}