//
// Created by HP on 2026/7/28.
//

#ifndef FASTSCREEN_FAA_SERVICE_SCHEDULER_H
#define FASTSCREEN_FAA_SERVICE_SCHEDULER_H
#include <string>
#include <vector>
#include <ws2def.h>

#include "faa_service_scheduler_bus.h"
#include "faa_service/faa_service_operation.h"
#include "faa_service/faa_service_task.h"
#include "faa_service/runtime/faa_service_runtime.h"
#include "faa_service/runtime/faa_servuce_runtime_init.h"

namespace faa::service::scheduler{

    template <typename O>
    void loop_to_udp(O& operation);

    template <typename O>
    void submit_short_task(O& operation,const SchedulerBus& scheduler_bus,std::string message,sockaddr_in client_addr);

    template <typename O>
    void submit_long_task(O& operation,const SchedulerBus& scheduler_bus);
}

template <typename O>
void faa::service::scheduler::loop_to_udp(O& operation){
    // auto net_work=operation.getNetwork();
    // auto& log =operation.getLog();
    // std::vector<char> buffer(net_work.config().buffer_size);
    // while(operation.running()){
    //     sockaddr_in client_addr{};
    //     std::string message;
    //     net_work.getMessage(buffer,message,client_addr);
    //     try {
    //         auto bus_info=nlohmann::json::parse(message);
    //         auto scheduler_bus=SchedulerBus();
    //         scheduler_bus.build_to_json(bus_info);
    //
    //         submit_long_task(operation,scheduler_bus);
    //         submit_short_task(operation,scheduler_bus,message,client_addr);
    //
    //     }catch (std::exception& e) {
    //
    //     }
    // }
}

template <typename O>
void faa::service::scheduler::submit_short_task(O& operation, const SchedulerBus& scheduler_bus,std::string message,sockaddr_in client_addr){
    // auto net_work=operation.getNetwork();
    // operation.getRunThreadPools().get_proc_thread_pools()->enqueue(
    //         [client_addr, message, scheduler_bus,&operation, net_work](){
    //             auto log =net_work.log();
    //             log.info("try process....");
    //             log.info("receive message:{}", message);
    //
    //             try {
    //                 log.info("list size:{}",scheduler_bus.mini_task_list.size());
    //                 for (const auto& [task_type, inputs, ocr_params] : scheduler_bus.mini_task_list){
    //                     switch (task::task_operation_map[task_type]){
    //                             case task::Win32_NoDet:{
    //                                     auto& controller=operation.getInputRes();
    //                                     for (const auto& it_input : inputs){
    //                                         if (!it_input.key.empty()){
    //                                             auto ret=operation::set_input_win32_key(
    //                                             scheduler_bus.target,controller.win32_inputs,
    //                                              {
    //                                                     .delay_time = it_input.delay_time,
    //                                                     .key_name   = it_input.key.c_str(),
    //                                                 }
    //                                             );
    //                                             if (!ret){
    //                                                 net_work.sendMessage("error",client_addr);
    //                                                 return;
    //                                             }
    //                                         }
    //                                         if (it_input.mouse_x>=0 && it_input.mouse_y>=0){
    //                                             auto ret=operation::set_input_win32_mouse(
    //                                             scheduler_bus.target,controller.win32_inputs,
    //                                              {
    //                                                     .mouse_x=it_input.mouse_x,
    //                                                     .mouse_y=it_input.mouse_y,
    //                                                      .is_client_coord = true,
    //                                                     .click_info = {
    //                                                         .count=it_input.count,
    //                                                         .delay_time = it_input.delay_time,
    //                                                         .click_type=static_cast<
    //                                                             input::windows::ClickMouseInfo::ClickType
    //                                                         >(it_input.click_type),
    //                                                     }
    //                                                 },
    //                                                 it_input.is_click
    //                                             );
    //
    //                                             if (!ret){
    //                                                 net_work.sendMessage("error",client_addr);
    //                                                 return;
    //                                             }
    //                                         }
    //                                         if (it_input.angleX >= 0 || it_input.angleY >= 0){
    //                                             auto ret=operation::set_rotate_win32_mouse(
    //                                             scheduler_bus.target,controller.win32_inputs,
    //                                              {
    //                                                     .sensitivity = it_input.sensitivity,
    //                                                     .angleX = it_input.angleX,
    //                                                     .angleY = it_input.angleY,
    //                                                     .step = it_input.step,
    //                                                     .delay_time = it_input.delay_time,
    //                                                 }
    //                                             );
    //                                             if (!ret){
    //                                                 net_work.sendMessage("error",client_addr);
    //                                                 return;
    //                                             }
    //                                         }
    //                                     }
    //                                     net_work.sendMessage("success",client_addr);
    //                                     break;
    //                             }
    //                             case task::GetImg:{
    //                                     operation::get_img(
    //                                         operation.getFrameRes().frame_mutex,
    //                                         scheduler_bus.target,
    //                                         operation.getFrameRes().frames
    //                                     );
    //                                     net_work.sendMessage("success",client_addr);
    //                                     break;
    //                             }
    //                             case task::GetOCR:{
    //                                   auto& ocrRes=operation.getOcrRes().peek_ocr(scheduler_bus.target);
    //                                   auto res=FaaSerOcrResult();
    //                                   operation::get_to_ocr(
    //                                       *ocrRes,
    //                                        operation.getOcrRes().ocr_mutex,
    //                                        scheduler_bus.target,
    //                                        operation.getFrameRes().frames ,
    //                                        fdnn::fcv::processer::ocr::FaaDetPostParam{
    //                                          .threshold = ocr_params.threshold,
    //                                          .unclip_ratio = ocr_params.unclip_ratio,
    //                                          .PAD = ocr_params.pad,
    //                                        }
    //                                       ,res
    //                                   );
    //                                   net_work.sendMessage(res.toString(),client_addr);
    //                                   break;
    //                             }
    //                             default: ;
    //                         }
    //                 }
    //
    //                 std::flush(std::cout);
    //           }catch (const std::exception& e){
    //               log.error("process error:{}", e.what());
    //               net_work.sendMessage("error",client_addr);
    //           }
    // });
}

template <typename O>
void faa::service::scheduler::submit_long_task(O& operation, const SchedulerBus& scheduler_bus) {

    // operation.getRunThreadPools().get_long_loop_thread_pools()->enqueue(
    //        [scheduler_bus,&operation](){
    //            auto status=operation.getWgcInfo().peek(scheduler_bus.target);
    //            if (status==1){
    //                operation.getWgcInfo().push(scheduler_bus.target);
    //                const auto wgc_cap=std::make_unique<rt::FaaWgcCapResources>();
    //                if (!rt::init::faa_wgc_init(*wgc_cap, scheduler_bus.target,operation.getLog())){
    //                    return;
    //                }
    //                operation::wgc_run(
    //                operation.getFrameRes().frame_mutex,
    //                operation.running(),
    //                scheduler_bus.target,
    //                operation.getFrameRes().frames,
    //                *wgc_cap
    //                );
    //            }
    //        });
}

#endif //FASTSCREEN_FAA_SERVICE_SCHEDULER_H
