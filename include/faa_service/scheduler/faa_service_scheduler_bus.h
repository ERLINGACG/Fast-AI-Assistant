//
// Created by HP on 2026/7/28.
//

#ifndef FASTSCREEN_FAA_SERVICE_SCHEDULER_BUS_H
#define FASTSCREEN_FAA_SERVICE_SCHEDULER_BUS_H
#include <nlohmann/json.hpp>

namespace faa::service::scheduler{
    struct SchedulerBus{

        std::string task_type;
        std::string target;

        struct input{
            int32_t mouse_x=-1;
            int32_t mouse_y=-1;
            std::string key;
            bool    is_click=false;
            int32_t delay_time=3000;

            int32_t click_type=0;
            int32_t count=1;

            double angleX=-1;
            double angleY=-1;
            double sensitivity=0.5;
            int32_t step=-1;

        };

        struct ocr_param{
            float threshold=0.5;
            float unclip_ratio=0.5;
            int32_t pad=2;
        };

        struct miniTask{
            std::string task_type;
            std::vector<input> inputs;
            ocr_param ocr_params{};
        } ;

        std::vector<miniTask> mini_task_list;

        auto build_to_json(const nlohmann::json& json){
            target   =json.value("target","");
            task_type=json.value("task_type","");
            if (json.contains("task_list")){
                for (const auto& it: json["task_list"]){
                    std::vector<input> inputs;
                    if (it.contains("input_list")){
                        for (const auto& it2: it["input_list"]){
                            auto i=input{
                                .mouse_x    =it2.value("mouse_x",-1),
                                .mouse_y    =it2.value("mouse_y",-1),
                                .key        =it2.value("key",std::string()),
                                .is_click   =it2.value("click",false),
                                .delay_time =it2.value("delay_time",3000),
                                .click_type =it2.value("click_type",0),
                                .count      =it2.value("count",1),
                                .angleX     =it2.value("angleX",-1.0),
                                .angleY     =it2.value("angleY",-1.0),
                                .sensitivity=it2.value("sensitivity",0.5),
                                .step       =it2.value("step",-1),
                            };
                            inputs.push_back(i);
                        }
                    }

                    miniTask mt=miniTask{
                        .task_type=it.value("task_type",""),
                        .inputs=inputs,
                    };
                    if (it.contains("ocr_params")){
                        mt.ocr_params={
                            .threshold    = static_cast<float>(it["ocr_params"].value("threshold",0.01)),
                            .unclip_ratio = static_cast<float>(it["ocr_params"].value("unclip_ratio",2.0)),
                            .pad          = it["ocr_params"].value("pad",5),
                        };
                    }
                    mini_task_list.push_back(mt);
                }
            }
        }
    };


}
#endif //FASTSCREEN_FAA_SERVICE_SCHEDULER_BUS_H