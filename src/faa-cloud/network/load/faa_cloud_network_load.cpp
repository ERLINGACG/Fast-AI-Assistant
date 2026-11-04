//
// Created by HP on 2026/10/22.
//
#include "faa_cloud/network/faa_cloud_network_load.h"
using faa::cloud::net_work::LoadHead;
using faa::cloud::net_work::LoadBody;
using faa::cloud::net_work::FaaCloudNetworkLoad;

void LoadBody::deserialize(const nlohmann::json& data){
    this->slice_len = data.value("slice_len",0);
    this->data      = data.value("body",nlohmann::json({}));

}

void LoadBody::push_back(const nlohmann::json& data) {

    this->slices.push_back(data);
    this->slice_len = slices.size();
}


nlohmann::json LoadBody::serialize() const
{
    nlohmann::json json;
    json["slice_len"] = this->slice_len;
    json["body"]      = this->data;
    return json;
}
void LoadBody::clear() {
    this->slices.clear();
    this->slice_len = 0;
}


