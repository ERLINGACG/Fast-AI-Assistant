//
// Created by HP on 2026/8/8.
//
#include "faa_service/service_event/operation/input/faa_service_win32_operation.h"

#include <iostream>

#include "faa-core/input/windows_input_code.h"
#include "faa_service/resources/faa_service_resources_input.h"
#include "faa_service/service_event/faa_service_event_task.h"

int faa::service::operation::input::send_key_inout_win32(const std::string& global_app,const nlohmann::json& info,
      resources::input::FaaWin32Inputs& inputs) {

      std::cout<<info.dump(2)<<std::endl;
      auto* input=inputs.peek(global_app);
      if(!input){
          return faa::input::windows::RunCode::process_error;
      }
      return input->simulateKeyPress({
                info.value("is_post",false),
            info.value("delay_time",3000),
             info["key"].get<std::string>().c_str(),
      });
}

int faa::service::operation::input::send_keys_input_win32(const std::string& global_app, const nlohmann::json& info,
    resources::input::FaaWin32Inputs& inputs){

    auto* input=inputs.peek(global_app);
    if(!input){
        return faa::input::windows::RunCode::process_error;
    }
    std::vector<faa::input::windows::InputKeyInfo> keys;
    std::vector<std::string> key_names;
    key_names.reserve(info["keys"].size());   // 预分配足够空间，避免扩容

    for (auto& key : info["keys"]) {
        key_names.emplace_back(key["key"].get<std::string>());
        keys.emplace_back(
            key.value("is_post", false),
            key.value("delay_time", 3000),
            key_names.back().c_str()
        );
    }
    return input->simulateKeys(keys);
}

int faa::service::operation::input::send_click_win32(const std::string& global_app, const nlohmann::json& info,
                                                     resources::input::FaaWin32Inputs& inputs) {

    auto* input=inputs.peek(global_app);
    if(!input){
        return faa::input::windows::process_error;
    }
        if (!info.contains("click")){
            return 0;
        }
        return input->clickMouse({
                    info.value("count",1),
                info.value("delay_time",1000),
            info.value("click","left_click")
        });

}

int faa::service::operation::input::move_mouse_win32(const std::string& global_app, const nlohmann::json& info,
    resources::input::FaaWin32Inputs& inputs)
{
    auto* input=inputs.peek(global_app);
    if(!input){
        return -2;
    }

    if (!info.contains("move_x") || !info.contains("move_y")){
        return 0;
    }
    const int32_t move_x        =info.value("move_x",0);
    const int32_t move_y        =info.value("move_y",0);
    const int32_t is_client_coord=info.value("is_client_coord",1);
    if(move_x<0 || move_y<0){
        return -3;
    }
    return input->moveMouse({
        move_x,
        move_y,
        1,
        is_client_coord,
    });


}

int faa::service::operation::input::rotate_mouse_win32(const std::string& global_app, const nlohmann::json& info,
    resources::input::FaaWin32Inputs& inputs)
{
    auto* input=inputs.peek(global_app);
    if(!input){
        return faa::input::windows::process_error;
    }
    auto angleX        =info.value("angleX",0.0);
    auto angleY        =info.value("angleY",0.0);
    return input->rotateMouse({
            info.value("sensitivity",10.0),
             angleX,
             angleY,
             info.value("step",10),
             info.value("step_delay",5)
    });
}

int faa::service::operation::input::drag_mouse_win32(const std::string& global_app, const nlohmann::json& info,
    resources::input::FaaWin32Inputs& inputs) {

    auto* input=inputs.peek(global_app);
    if(!input){
        return faa::input::windows::process_error;
    }
    return input->dragMouse({
        info.value("click","left_click"),
          info.value("move_to_x",0),
          info.value("move_to_y",0),
             info.value("step",10),
        info.value("step_delay",5)
    });
}

void faa::service::operation::input::send_input(event::Ack& ack, const event::Task& task,const nlohmann::json& info,
                                                resources::input::FaaInputResources& inputs) {

        switch (event::controllers_map[task.controller]){
                case event::WIN32_C:{
                    if (!info.contains("input")){
                        break;
                    }
                    int final_ret = 0;
                    nlohmann::json detail_info;
                    for (auto& it: info["input"]){
                        int ret=0;
                        if (it.contains("key")){
                            ret=send_key_inout_win32(task.global_app,it,inputs.win32_inputs);
                            if(ret!=0) final_ret=ret;
                        }

                        if (it.contains("keys")){
                            ret=send_keys_input_win32(task.global_app,it,inputs.win32_inputs);
                            if(ret!=0) final_ret=ret;
                        }

                        if (it.contains("move_x") || it.contains("move_y")){
                            ret=move_mouse_win32(task.global_app,it,inputs.win32_inputs);
                            if(ret!=0) final_ret=ret;
                        }

                        if (it.contains("angleX") || it.contains("angleY")){
                            ret=rotate_mouse_win32(task.global_app,it,inputs.win32_inputs);
                            if(ret!=0) final_ret=ret;
                        }

                        if (it.contains("move_to_x") || it.contains("move_to_y")){
                            ret=drag_mouse_win32(task.global_app,it,inputs.win32_inputs);
                            if(ret!=0) final_ret=ret;
                        }

                        if (it.contains("click")){
                            ret=send_click_win32(task.global_app,it,inputs.win32_inputs);
                            if(ret!=0) final_ret=ret;
                        }
                    }
                    ack.code=final_ret;
                    if (final_ret!=0) {
                        ack.starts="false";
                        ack.error_info="param is invalid";
                    }
                    break;
                }
                default:
                    break;
        }
}
