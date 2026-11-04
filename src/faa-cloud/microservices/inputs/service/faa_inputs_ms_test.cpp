//
// Created by HP on 2026/10/28.
//
#include "faa_cloud/microservices/inputs/service/faa_inputs_ms.h"

#include "faa_core/input/keyboard/faa_keyboard_params_build.h"
#include "faa_core/input/mouse/faa_mouse_params_build.h"

using faa::cloud::microservices::inputs::FaaCloudInputMS;
void FaaCloudInputMS::addActionOperat() {
    if (log) log->info("addActionOperat");
    this->operat.push("action",[this](net_work::FaaCloudNetworkRequest& request){

        base::service::netWorkSend(
            this->network,request,
            [this](net_work::FaaCloudNetworkRequest& request){

                         try{
                              // // this->keyboardInput.resetHwnd(tag_data.value("tag",""));
                              if (this->keyboardInput.isInit()) {
                                   this->keyboardInput.active();
                                   return;
                              }
                             request.deserializeBody(nlohmann::json{
                                    {"body", {{"error", "keyboard input tag not init"}}}
                             });
                          }catch(const std::exception& e){
                              log->error("reset window handle failed: {}", e.what());
                          }
            },
            this->log
        );

        return 0;
    });
}

void FaaCloudInputMS::addKeyboardApplyOperat()
{
    if (log) log->info("addKeyboardApplyOperat");
    this->operat.push("keyboard_apply",[this](net_work::FaaCloudNetworkRequest& request){

       base::service::netWorkSend(
           this->network,request,
           [this](net_work::FaaCloudNetworkRequest& request){
                        try{
                             auto message = core::input::FaaReturnMessage();

                             const auto params  = core::input::keyboard::json_to_faa_keyboard_params(
                                 request.serializeBody().value(
                                 "body",nlohmann::json::object()
                                 )
                             );

                             this->keyboardInput.apply(params,message);
                             auto return_message = nlohmann::json::object();

                             if (message.error.empty()){
                                 return_message["body"]["message"] = "success";
                             }else{
                                 return_message["body"]["message"] = message.error;
                             }
                             request.setBody(return_message);


                         }catch(const std::exception& e){
                             log->error("reset window handle failed: {}", e.what());
                         }
           },
           this->log
       );

       return 0;
   });
}
void FaaCloudInputMS::addMouseApplyOperat   () {
    if (log) log->info("addMouseApplyOperat");
    this->operat.push("mouse_apply",[this](net_work::FaaCloudNetworkRequest& request){

       base::service::netWorkSend(
           this->network,request,
           [this](net_work::FaaCloudNetworkRequest& request){
                        try{
                             auto message = core::input::FaaReturnMessage();

                             const auto params  = core::input::mouse::json_to_faa_mouse_params(
                                 request.serializeBody().value(
                                 "body",nlohmann::json::object()
                                 )
                             );

                             for (auto& param : params) {
                                 this->mouseInput.apply(param,message);
                                 if (!message.error.empty()) {
                                      break;
                                 }
                             }
                             auto return_message = nlohmann::json::object();

                             if (message.error.empty()){
                                 return_message["body"]["message"] = "success";
                             }else{
                                 return_message["body"]["message"] = message.error;
                             }
                             request.setBody(return_message);


                         }catch(const std::exception& e){
                             log->error("reset window handle failed: {}", e.what());
                         }
           },
           this->log
       );

       return 0;
   });
}

