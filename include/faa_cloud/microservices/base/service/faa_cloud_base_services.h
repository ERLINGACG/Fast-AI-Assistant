//
// Created by HP on 2026/10/9.
//

#ifndef FASTSCREEN_FAA_CLOUD_BASE_SERVICES_H
#define FASTSCREEN_FAA_CLOUD_BASE_SERVICES_H
#include <nlohmann/json.hpp>

#include "faa_cloud/network/faa_cloud_network.h"
#include"faa_core/faa_export.h"
#include "info/ilog/ilog.h"
#include "faa_cloud/microservices/base/resource/faa_cloud_resource.h"
#include "faa_cloud/command/faa_command_tree.h"
#include "faa_core/resources/faa_cv_resources.h"
#include "faa_core/thread_pool/faa_thread_pool.h"

namespace faa::cloud::microservices::base {


    template<typename T>
    class FAA_CPP_EXPORT FaaCloudBaseServices {

    public:

            const char* isRunning    = "isRunning";  // 类型是 const char[9]

    protected:

        nlohmann::json config{};


        command::FaaCommandTree cmd{};
        core::thread_pool::FAAThreadPool thread_pool{};
        resource::SignalMap  signals{};
        resource::Network    network{};
        resource::ThreadsMap threads{};


        resource::OperatMap<
            net_work::FaaCloudNetworkRequest&
        > operat{};

    public:
        T& setConfig        (nlohmann::json& config,info::log::Ilog* log = nullptr);
        T& setNetwork(std::unique_ptr<net_work::FaaCloudNetwork> network,info::log::Ilog* log = nullptr);

        T& setSignal     (const std::string& name,bool value);
        T& registerDefine(info::log::Ilog* log = nullptr);

        T& init ();
        T& start(info::log::Ilog* log = nullptr);

        T& close();
        T& stop ();


        T& addThread(
            const std::string& name,
            std::thread thread,
            resource::ThreadsMap::running_type type,
            info::log::Ilog* log = nullptr);

        T& addCommand(
            std::unique_ptr<command::FaaCommandTreeNode> cmd,
            const std::vector<std::string>& path,
            info::log::Ilog* log = nullptr
        );


        resource::SignalMap&  getSignalMap()   { return signals;};
        resource::ThreadsMap& getThreadsMap()  { return threads;};




    };



}
#include "./faa_cloud_base_imp.h"
#include "./faa_cloud_base_network.h"
#endif //FASTSCREEN_FAA_CLOUD_BASE_SERVICES_H