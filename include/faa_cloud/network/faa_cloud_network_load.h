//
// Created by HP on 2026/9/15.
//

#ifndef FASTSCREEN_FAA_CLOUD_NETWORK_RELOAD_H
#define FASTSCREEN_FAA_CLOUD_NETWORK_RELOAD_H
#include <ws2def.h>
#include <ws2tcpip.h>
#include <nlohmann/json.hpp>

namespace faa::cloud::net_work {


    struct LoadHead {

        nlohmann::json data = nlohmann::json({});

        std::string operation;
        std::string router_to;
        std::string request_ip;
        uint16_t    request_port;
        int         slice_len;


        auto serialize(const sockaddr_in& addr,const nlohmann::json& data){
            char ip_str[INET_ADDRSTRLEN];
            const std::string requester_ip(inet_ntop(AF_INET,&(addr.sin_addr), ip_str,INET_ADDRSTRLEN));
            const uint16_t requester_port = ntohs(addr.sin_port);

            this->operation   =data.value("operation","");
            this->router_to   =data.value("router_to","");
            this->request_ip  =requester_ip;
            this->request_port=requester_port;

            this->data["operation"]   =operation;
            this->data["router_to"]    =router_to;
            this->data["request_ip"]   =request_ip;
            this->data["request_port"] =request_port;
            this->data["slice_len"]    =slice_len;

        }

        auto serialize(const nlohmann::json& data){

            this->data = data;
            operation     =data.value("operation","");
            router_to     =data.value("router_to","");
            request_ip    =data.value("request_ip","");
            request_port  =data.value("request_port",0);
            slice_len     =data.value("slice_len",0);

            this->data["operation"]=operation;
            this->data["router_to"]=router_to;
            this->data["request_ip"]=request_ip;
            this->data["request_port"]=request_port;
            this->data["slice_len"]=slice_len;

        }

        auto set_slice_len(int len){
            slice_len = len;
            this->data["slice_len"]=slice_len;
        }




    };
    struct LoadBody {

        int slice_len;
        nlohmann::json data;
        std::vector<nlohmann::json> slices;

        auto serialize(const nlohmann::json& data){
            this->slice_len = data.value("slice_len",0);
            this->data      = data.value("body",nlohmann::json({}));
        }

        auto addSlice(const nlohmann::json& data){
            auto ids = "slice_"+std::to_string(slices.size());
            slices.push_back(
                {
                        {ids,data}
                       }
            );
            slice_len ++;
            return *this;
        }

        auto setBody(const nlohmann::json& data){
            this->data = data;
        }

        auto clear(){
             slice_len = 0;
             slices.clear();
             data = nlohmann::json({});
             return *this;
        }
    };

    struct  FaaCloudNetworkLoad {

        LoadHead head_;
        LoadBody body_;

        auto serialize(const sockaddr_in& addr,const nlohmann::json& data){
            head_.serialize(addr,data);
            body_.serialize(data);
            return *this;
        }

        auto serialize(const nlohmann::json& data){
            head_.serialize(data);
            body_.serialize(data);
            return *this;
        }

        auto setBody(const nlohmann::json& data){
            body_.setBody(data);
            return *this;
        }

        auto addSlice(const nlohmann::json& data){
            body_.addSlice(data);
            return *this;
        }

        [[nodiscard]] auto unserialize(){

            head_.set_slice_len(body_.slice_len);
            auto response = nlohmann::json(head_.data);

            if(head_.slice_len == 0){
                response["body"]= body_.data;
            }
            return response;
        }

        auto& head(){
            return head_;
        }

        auto& body(){
            return body_;
        }

        auto clearBody(){
            body_.clear();
        }



    };
}
#endif //FASTSCREEN_FAA_CLOUD_NETWORK_RELOAD_H