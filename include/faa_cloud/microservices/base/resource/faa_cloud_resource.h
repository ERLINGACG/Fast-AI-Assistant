//
// Created by HP on 2026/10/9.
//

#ifndef FASTSCREEN_FAA_CLOUD_RESOURCE_H
#define FASTSCREEN_FAA_CLOUD_RESOURCE_H
#include <map>
#include <mutex>
#include <string>
#include <thread>

#include "faa_cloud/common/frame/faa_cloud_frame.h"
#include "faa_cloud/network/faa_cloud_network.h"

namespace faa::cloud::microservices::base::resource
{
    struct Network {
        std::unique_ptr<net_work::FaaCloudNetwork> network;
        std::string identity;

        [[nodiscard]] auto getIdentity() const {
            return identity;
        }

        [[nodiscard]] auto& get() const{
            return *network;
        }

    };



    struct ThreadsMap {

        std::mutex mutex;

        enum running_type {
            join,
            detach,
            none,
        };

        enum running_status {
            ready,
            running,
            stopped,
        };

        struct thread_c {

            running_type type     = join;
            running_status status = ready;
            std::thread thread;

        };
        std::map<std::string,thread_c> threads;
    };

    struct SignalMap {

        std::mutex mutex;
        std::map<std::string,std::atomic<bool>> signals;

        auto set(const std::string& signal, const bool value){
            std::lock_guard lock(mutex);
            signals[signal]=value;
        }

        [[nodiscard]] auto get(const std::string& signal)  {
            std::lock_guard lock(mutex);
            const auto s=signals.find(signal);
            if (s==signals.end()) {
                return false;
            }
            return s->second.load();
        }
    };


    template<typename... Args>
    struct OperatMap {
        std::map<std::string,std::function<int(Args...)>> map{};
        auto push(const std::string& name, std::function<int(Args...)> func) {
            map[name]=func;
        }
    };







}
#endif //FASTSCREEN_FAA_CLOUD_RESOURCE_H