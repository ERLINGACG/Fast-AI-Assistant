//
// Created by HP on 2026/8/5.
//

#ifndef FASTSCREEN_FAA_SERVICE_EVENT_TASK_H
#define FASTSCREEN_FAA_SERVICE_EVENT_TASK_H
#include <map>
#include <string>
#include <nlohmann/json.hpp>

namespace faa::service::event{

    enum CapType{
        WGC=1,
        ADB=2,
    };
    enum ControllerType{
        WIN32_C=0x13,
        ADB_C  =0x14,
    };

    enum EventType{
        SAVE_FRAME=1,
        OCR_TO_IMG=2,
        SEND_INPUT=3,
    };
    inline  std::map<std::string,ControllerType> controllers_map{
             {"win32",WIN32_C},
             {"adb",ADB_C},
         };

    inline std::map<std::string,CapType> caps_map{
             {"wgc",WGC},
             {"adb",ADB},
    };
    inline std::map<std::string,EventType> event_map{
             {"save_frame",SAVE_FRAME},
             {"ocr_img",OCR_TO_IMG},
             {"send_input",SEND_INPUT},
    };

    struct EventNode{
         std::string event_type;
         nlohmann::json event_description;
    };

    struct Task{


         std::string global_app;
         std::string controller;
         std::string cap_type;
         nlohmann::json meta_data;

         auto buildTask(const nlohmann::json& json){
             global_app = json.value("global_app","");
             controller = json.value("controller","");
             cap_type   = json.value("cap_type","");
             meta_data=json;
         }
         auto toString(){

             return nlohmann::json{
                 {"global_app",global_app},
                 {"controller",controller},
                 {"cap_type",cap_type},
             };
         }

         [[nodiscard]] auto toEventList()const{
             if (meta_data.contains("event_list")){
                 std::vector<EventNode> event_list;
                 for (const auto& it : meta_data["event_list"]){
                     event_list.push_back(EventNode{
                         it.value(     "event_type",""),
                         it.value("description",nlohmann::json{})
                     });
                 }
                 return event_list;
             }
             return std::vector<EventNode>{};
         }
    };
}
#endif //FASTSCREEN_FAA_SERVICE_EVENT_TASK_H