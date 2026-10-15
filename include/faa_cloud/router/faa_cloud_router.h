//
// Created by HP on 2026/8/24.
//

#ifndef FASTSCREEN_FAA_CLOUD_ROUTER_H
#define FASTSCREEN_FAA_CLOUD_ROUTER_H
#include "faa_core/thread_pool/faa_thread_pool.h"
#include "faa_cloud/network/faa_cloud_network.h"

namespace  faa::cloud::router{


    struct MsInfo{
        int32_t id{};
        std::string name;
        std::string state;
        sockaddr_in addr{};
    };

    struct RegisterMapper{

        std::map<int32_t,MsInfo> map;
        std::mutex mutex;


        auto registerMs(const int32_t id, const std::string& name, const sockaddr_in addr){
            std::lock_guard lock(mutex);
            if (map.contains(id)){
                return ;
            }
            map[id]={id,name,"online",addr};
        }

        auto unregisterMs(const int32_t id){
            std::lock_guard lock(mutex);
            if (map.contains(id)){
                // map.erase(id);
                map[id].state="offline";
            }
        }

        auto getMsName(const int32_t id){
            std::lock_guard lock(mutex);
            return map.contains(id)?map[id].name:"";
        }

        auto getMsInfo(const std::string& name){
            std::lock_guard lock(mutex);
            for (const auto& val : map | std::views::values){
                if (val.name==name){
                    return val;
                }
            }
            return MsInfo{0,"",
                "offline",
                {0,0,0,0}
            };
        }

        auto getInfo(){
            std::lock_guard lock(mutex);
            nlohmann::json ms_info;
            for (const auto& val : map | std::views::values){
                ms_info[val.id]={
                    {"id",  val.id},
                    {"name",val.name},
                    {"state",val.state}
                };
            }
            return ms_info;
        }



    };

    class FaaCloudRouter {


        std::unique_ptr<core::thread_pool::FAAThreadPool> thread_pool_;
        std::unique_ptr<net_work::FaaCloudNetwork> network;
        std::thread       run_thread;
        std::thread       cmd_thread;

        std::atomic<bool> running{true};
        std::atomic<int32_t> id{0};

        RegisterMapper register_mapper{};

        std::map<std::string,std::function<bool(nlohmann::json& requires_detail, sockaddr_in& addr)>> operator_map{
        };
        std::map<std::string,std::function<bool (net_work::FaaCloudNetworkLoad& load, sockaddr_in& addr)>> operator_map_v2{
        };


        public:
            explicit FaaCloudRouter(std::unique_ptr<net_work::FaaCloudNetwork> network) : network(std::move(network)){}
            FaaCloudRouter& initThreadPool();
            FaaCloudRouter& initOperatorHandlerV2();
            FaaCloudRouter& start(info::log::Ilog& logger);
            FaaCloudRouter& join();


            bool responseV2   (net_work::FaaCloudNetworkLoad& load, sockaddr_in& addr);
            bool queryMsInfoV2(net_work::FaaCloudNetworkLoad& load, sockaddr_in& addr);
            bool routeMsInfoV2(net_work::FaaCloudNetworkLoad& load, sockaddr_in& addr);
            bool registerMsV2 (net_work::FaaCloudNetworkLoad& load, sockaddr_in& addr);


            void loop(info::log::Ilog& logger);
            void cmd_loop(info::log::Ilog& logger);
    };
}
#endif //FASTSCREEN_FAA_CLOUD_ROUTER_H