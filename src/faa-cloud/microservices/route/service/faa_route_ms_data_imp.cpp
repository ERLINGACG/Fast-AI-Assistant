//
// Created by HP on 2026/10/24.
//
#include "faa_cloud/microservices/route/service/faa_route_ms_data.h"
using faa::cloud::microservices::route::service::RegisterMsInfo;
using faa::cloud::microservices::route::service::RegisterMsSet;

nlohmann::json RegisterMsInfo::serialize() {
    return nlohmann::json::object({
        {"identity",identity},
        {"timestamp",timestamp},
        {"type",type},
        {"status",status},
    });
}
bool RegisterMsInfo::equals(const RegisterMsInfo& info) const {
    if (info.host == this->host &&
        info.port == this->port)
    {
        return true;
    }
    return  false;
}

bool RegisterMsSet::push(RegisterMsInfo& info,info::log::Ilog* log) {

    std::lock_guard lock(mutex);

    for (auto& val : ms_info | std::views::values){

           if (val.equals(info)) {
              val.status = "online";
              info.identity = val.identity;
              log->debug("resurrection to {}", val.identity);
              return true;
           }

    }
    if (const auto ms_define = info.identity + "_" + std::to_string(this->idx); !ms_info.contains(ms_define)){
        info.status   = "online";
        info.identity = ms_define;
        ms_info[ms_define] = info;
        ++this->idx;
        log->info("push ms info: {} success", ms_define);
        return true;
    }

    return false;
}

bool RegisterMsSet::update(const RegisterMsInfo& info,info::log::Ilog* log) {
    return true;
}

bool RegisterMsSet::remove(const std::string& identity,info::log::Ilog* log) {
    return true;
}

bool RegisterMsSet::query(const std::string& identity, RegisterMsInfo& info)
{
    std::lock_guard lock(mutex);
    auto ms = ms_info.find(identity);
    if (ms == ms_info.end()){
        info.identity = "unknown";
        info.status   = "unregistered";
        return false;
    }
    info = ms->second;
    return true;
}



