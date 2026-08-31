//
// Created by HP on 2026/8/5.
//

#ifndef FASTSCREEN_FAA_SERVICE_TASK_OPERATION_H
#define FASTSCREEN_FAA_SERVICE_TASK_OPERATION_H
#include "faa_service_frame_operation.h"
#include "faa_service_wgc_operation.h"
#include "faa_service/faa_service_operation.h"
#include "faa_service/resources/faa_service_resources.h"
#include "faa_service/runtime/faa_servuce_runtime_init.h"
#include "faa_service/service_event/faa_service_event_ack.h"
#include "faa_service/service_event/faa_service_event_task.h"
#include "input/faa_service_win32_operation.h"
#include "ocr/faa_service_ocr_operation.h"
// #include "faa_service/service_event/operation/faa_service_wgc_operation.h"
namespace faa::service::operation {
    template<typename T>
    void submit_long_task(T& t, event::Task task);

    template<typename T>
    void submit_short_task(T& t, event::Task task,sockaddr_in client_addr);

}

template<typename T>
void faa::service::operation::submit_long_task(T& t, event::Task task){

        auto& resources=t.getResources();

        resources.thread_pools.long_task->enqueue([&t,task, &resources](){

            switch (event::caps_map[task.cap_type]){
                case event::CapType::WGC:{
                        operation::wgc_loop_v2(t,task,resources,60);
                        std::cout<<std::endl;
                    break;
                }
                default:
                    break;
            }
            std::cout<<std::flush;
        });
    }

template <typename T>
void faa::service::operation::submit_short_task(T& t, const event::Task task,sockaddr_in client_addr){
    auto& resources=t.getResources();
    auto  log=t.getLog();

    resources.thread_pools.short_task->enqueue([&t, &resources, &log, task,&client_addr](){
        log.info("Submit short task list: {}",task.toEventList().size());
        auto ack=event::Ack
        {
            0,
            "success",
            ""
        };
        for (const auto& [event_type,event_description] : task.toEventList()){
            switch (event::event_map[event_type]){
                case event::EventType::SAVE_FRAME:{
                    log.info("Save frame");
                    operation::save_frame(resources,task.global_app,"");
                    break;
                }
                case event::EventType::OCR_TO_IMG:{
                    ocr::OcrAckResult result0;
                    resources.ocr_dnn_resource.push(task.global_app,t.getMetaConfig());

                    ocr::get_to_ocr_det(const_cast<nlohmann::json&>(event_description),resources,task.global_app,result0,
                        [&t,&result0,&log,&client_addr,&ack](){
                            nlohmann::json detail_obj;
                            detail_obj["ocr"] = result0.toData();
                            ack.detail(detail_obj);
                        }
                    );
                    break;
                }
                case event::EventType::SEND_INPUT:{
                    input::send_input(ack,task,event_description,resources.input_resources,
                        [&t,&ack,&resources,&log,&client_addr,&task](){
                           ack.detail();

                            }
                    );
                }
                default:
                    break;
            }
        }
        t.getNetwork().sendMessage(ack.toString(),client_addr);
        std::cout<<std::flush;
    });
}
#endif //FASTSCREEN_FAA_SERVICE_TASK_OPERATION_H
