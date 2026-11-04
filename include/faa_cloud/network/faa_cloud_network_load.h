//
// Created by HP on 2026/9/15.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CLOUD_NETWORK_LOAD_H
#define FASTAIASSISTANT_ROOT_FAA_CLOUD_NETWORK_LOAD_H
#include <nlohmann/json.hpp>

namespace faa::cloud::net_work {


    struct LoadHead {

        nlohmann::json data = nlohmann::json({});

        std::string operation;
        std::string router_to;
        std::string request_ip;
        uint16_t    request_port;
        std::string br_host;
        uint16_t    br_port;
        int         slice_len;

        auto deserialize(const nlohmann::json& data){

            // this->data = data;
            operation     = data.value("operation","");
            router_to     = data.value("router_to","");
            request_ip    = data.value("request_ip","");
            request_port  = data.value("request_port",0);
            slice_len     = data.value("slice_len",0);

            br_host     = data.value("br_host","");
            br_port     = data.value("br_port",0);

            this->data["operation"]=operation;
            this->data["router_to"]=router_to;
            this->data["request_ip"]=request_ip;
            this->data["request_port"]=request_port;
            this->data["slice_len"]=slice_len;
            this->data["br_host"]=br_host;
            this->data["br_port"]=br_port;

        }
    };
    struct LoadBody {

        int slice_len;
        nlohmann::json data;
        std::vector<nlohmann::json> slices;

        void deserialize(const nlohmann::json& data);
        void push_back(const nlohmann::json& data);


        [[nodiscard]] nlohmann::json serialize() const;
        void clear();


    };

    struct  FaaCloudNetworkLoad {

        LoadHead head;
        LoadBody body;

        [[nodiscard]] auto& getHead() const {
            return head;
        }
        [[nodiscard]] auto& getBody() const{
            return body;
        }

        void setBody(const nlohmann::json& data){
            this->body.data = data;
        }

        auto clearBody(){
            body.clear();
        }


    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_CLOUD_NETWORK_LOAD_H