//
// Created by HP on 2026/10/13.
//
#include "faa_cloud/microservices/general/service/faa_cloud_general.h"
#include "faa_core/win_utils/faa_win_utils.h"
using faa::cloud::microservices::general::FaaCloudGeneralServices;

void FaaCloudGeneralServices::addWin32InputOper(info::log::Ilog* log) {
    this->operat.push("win32_input",
        [this,log] (net_work::FaaCloudNetworkLoad& load,sockaddr_in response_addr){

                auto requires_info = load.body().data;
                log->debug("win32_input to {}",requires_info.dump());
                auto target = requires_info.value("target",this->cap.getTarget());

                if(target.empty()){
                    load.setBody({"error","target is empty"});
                    this->network.get().sendLoad(
                        load,
                        response_addr
                    );
                    return 0;
                }

                if (!this->win32_input.isInit() || this->win32_input.getTarget()!=target){
                      auto hwnd=core::win_utils::GetWindowHandle(target);
                      if (hwnd==nullptr){
                          hwnd=core::win_utils::GetWindowHandleByClass(target);
                      }
                      if (hwnd==nullptr){
                          hwnd=core::win_utils::GetWindowHandleByName(target);
                      }
                      if (hwnd==nullptr){
                          log->error("get window handle failed");
                          load.setBody({"error","get window handle failed"});
                          this->network.get().sendLoad(
                              load,
                              response_addr
                          );
                          return 0;
                      }
                      log->debug("hwnd is found");
                      this->win32_input.reset(
                          std::make_unique<input::windows::WindowsInput>(hwnd),target
                      );
                 }

                 if (this->win32_input.isInit()){
                     for(auto& it:requires_info.value("inputs",nlohmann::json::array())){
                        auto type_name= it.value("type","");
                          if (this->win32_input.operators.contains(type_name) ){
                              this->win32_input.operators[type_name](it);
                          }
                     }

                 }
                 load.setBody({"success"});
                 this->network.get().sendLoad(
                    load,
                    response_addr
                );
                return 0;
        }
    );

}