//
// Created by HP on 2026/8/6.
//

#ifndef FASTSCREEN_FAA_SERVICE_WIN32_OPERATION_H
#define FASTSCREEN_FAA_SERVICE_WIN32_OPERATION_H
#include "faa_core/input/windows_input_code.h"
#include "faa_service/resources/faa_service_resources_input.h"
#include "faa_service/service_event/faa_service_event_ack.h"
#include "faa_service/service_event/faa_service_event_task.h"

namespace faa::service::operation::input{

     inline std::map<faa::input::windows::RunCode,std::string> error_mapping{
          {faa::input::windows::RunCode::process_error,"process not found"},
          {faa::input::windows::RunCode::key_not_found,"key not found"},
     };

     int send_key_inout_win32 (const std::string& global_app,const nlohmann::json& info, resources::input::FaaWin32Inputs& inputs);
     int send_keys_input_win32(const std::string& global_app,const nlohmann::json& info, resources::input::FaaWin32Inputs& inputs);
     int send_click_win32     (const std::string& global_app,const nlohmann::json& info, resources::input::FaaWin32Inputs& inputs);
     int move_mouse_win32     (const std::string& global_app,const nlohmann::json& info, resources::input::FaaWin32Inputs& inputs);
     int rotate_mouse_win32   (const std::string& global_app,const nlohmann::json& info, resources::input::FaaWin32Inputs& inputs);
     int drag_mouse_win32     (const std::string& global_app,const nlohmann::json& info, resources::input::FaaWin32Inputs& inputs);


     void send_input(event::Ack& ack, const event::Task& task,const nlohmann::json& info, resources::input::FaaInputResources& inputs);

     template<typename F>
     void send_input(event::Ack& ack, const event::Task& task, const nlohmann::json& info, resources::input::FaaInputResources& inputs,F&& f){
          send_input(ack,task,info,inputs);
          f();
     }
}
#endif //FASTSCREEN_FAA_SERVICE_WIN32_OPERATION_H