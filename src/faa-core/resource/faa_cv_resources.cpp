//
// Created by HP on 2026/10/17.
//
#include "faa_core/resources/faa_cv_resources.h"

using faa::core::fcv::resources::FaaCvResource;
using faa::core::fcv::resources::FaaCvResItem;

FaaCvResource& FaaCvResource::load(info::log::Ilog* ilog) {
    std::cout << "load item: " << res_config.dump() << std::endl;

    // 顶层必须有 "globals"
    if (!res_config.contains("globals") || !res_config["globals"].is_object()) {
        if (ilog) ilog->warn("FaaCvResource::load: no 'globals' object");
        return *this;
    }

    const auto& globals = res_config["globals"];

    // globals = {"ui_test1": {"list": [...]}, ...}
    for (const auto& [global_name, global_value] : globals.items()) {
        if (!global_value.contains("list") || !global_value["list"].is_array()) {
            if (ilog) ilog->warn("FaaCvResource::load: '{}' has no 'list' array", global_name);
            continue;
        }

        const auto& list = global_value["list"];

        for (const auto& j : list) {
            auto item_name = j.value("name", "");
            auto item_path = j.value("path", "");
            if (item_name.empty() || item_path.empty()) {
                if (ilog) ilog->warn("FaaCvResource::load: invalid item in '{}'", global_name);
                continue;
            }

            this->data[global_name].push_back(
                FaaCvResItem{
                    .name = item_name,
                    .path = item_path,
                }.read()
            );
            if (ilog) ilog->debug("FaaCvResource::load: load item: {} from path: {}", item_name, item_path);
        }
    }
    return *this;
}
FaaCvResource& FaaCvResource::setConfig(const nlohmann::json& config){
    res_config = config;
    return *this;
}

bool FaaCvResource::query(const std::string& global_name, const std::string& item_name,cv::Mat& image){
    if (!this->data.contains(global_name)) return false;

    for (const auto& item : this->data[global_name]){
        if (item.name == item_name) {
            image = item.image;
            return true;
        }
    }
    return false;
}
