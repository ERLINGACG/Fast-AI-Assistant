//
// Created by HP on 2026/6/29.
//
#include "faa_service/faa_service.h"
#include <ws2tcpip.h>
#include <opencv2/highgui.hpp>

#include "faa_core/faa_data.h"
#include "faa_core/win_utils/faa_win_utils.h"
#include "faa-utlis/fs_json_utils.h"
#include "faa_service/faa_service_operation.h"
#include "faa_service/faa_service_task.h"
#include "../../include/faa_service/runtime/faa_servuce_runtime_init.h"

int faa::service::FAAServiceUDP::resource_config(std::string config_path){
    try{
        std::ifstream file(config_path);
        if (!file.is_open()) {
            log.error("Cannot open config file: {}", config_path);
            return -1;
        }
        file >> resources_config_json;   // 直接用 operator>> 读取 JSON
        // if (resources_config_json.contains("ocr_config")){
        //     model_resource_config.ocr_det_path=resources_config_json["ocr_config"]["ocr_det_path"];
        //     model_resource_config.ocr_rec_path=resources_config_json["ocr_config"]["ocr_rec_path"];
        //     log.debug("model_resource_config.ocr_det_path: {}", model_resource_config.ocr_det_path);
        //     log.debug("model_resource_config.ocr_rec_path: {}", model_resource_config.ocr_rec_path);
        //
        // }
        file.close();
    }catch (std::exception& e){
        this->log.error("parse config file failed: {}", e.what());
        return -1;
    }
    return 0;
}

int faa::service::FAAServiceUDP::init() {
    this->log.info("FastScreenFsService init to {}:{}",
        this->config.ip,
        this->config.start_port
    );

    auto wsa_startup = [&](){
        if (WSAStartup(MAKEWORD(2, 2), &this->runtime.wsaData) != 0) {
            this->log.error("WSAStartup failed: {}", WSAGetLastError());
            return false;
        }
        this->log.info("WSAStartup success");
        return true;
    };


    auto wsa_create_socket = [&]() {
        this->runtime.server_socket= socket(AF_INET, SOCK_DGRAM, 0);
        if (this->runtime.server_socket == INVALID_SOCKET) {
            this->log.error("socket create failed: {}", WSAGetLastError());
            WSACleanup();
            return false;
        }
        this->log.info("socket create success");
        return true;
    };

    auto bind_to_socket = [&]() {

        this->runtime.server_addr.sin_family = AF_INET; // IPv4
        this->runtime.server_addr.sin_addr.s_addr = htonl(INADDR_ANY);          // 监听所有本地 IP[reference:13]
        this->runtime.server_addr.sin_port = htons(this->config.start_port);   // 设置端口[reference:14]

        if (bind(this->runtime.server_socket,
          reinterpret_cast<struct sockaddr*>(&this->runtime.server_addr),
          sizeof(this->runtime.server_addr)) == SOCKET_ERROR) {
            // 处理错误
            this->log.error("bind failed: {}", WSAGetLastError());
            closesocket(runtime.server_socket); // 关闭套接字
            WSACleanup();
            return false;
        }

        this->log.info("bind success");
        return true;
    };

    try {

        if (!wsa_startup())       return -1;
        if (!wsa_create_socket()) return -1;
        if (!bind_to_socket())    return -1;

    }catch (std::exception& e) {
        this->log.error("FastScreenFsService init error: {}", e.what());
    }
    return 0;
}

int faa::service::FAAServiceUDP::run_loop() {
    std::vector<char> buffer(this->config.buffer_size);
    sockaddr_in client_addr{};
    int client_addr_len = sizeof(client_addr);
    char client_ip[46]; // 固定大小

    this->log.info("UDP start ...");

    while (this->is_running.load()) {
        int bytes_received = recvfrom(
            this->runtime.server_socket,
            buffer.data(),
            static_cast<int>(buffer.size()) - 1,
            0,
            reinterpret_cast<struct sockaddr*>(&client_addr),
            &client_addr_len
        );

        if (bytes_received == SOCKET_ERROR) {
            int error = WSAGetLastError();

            if (error == WSAECONNRESET) continue;
            if (error == WSAENOTSOCK || error == WSAEINTR) {
                // 不打印错误，直接退出循环
                this->log.info("the loop is exit");
                break;
            }
            this->log.error("recvfrom failed: {}", error);
            break;
        }

        buffer[bytes_received] = '\0';
        std::string json_str(buffer.data(), bytes_received);
        sockaddr_in client_copy = client_addr;


        this->thread_pool->enqueue([this, json_str = std::move(json_str), client_copy](){
           char ip[46];
           inet_ntop(AF_INET, &client_copy.sin_addr, ip, sizeof(ip));
           int port = ntohs(client_copy.sin_port);
           this->log.info("recvfrom {}:{} message: {}", ip, port, json_str);

           auto data=core::data::EvenTStruct{};

           utils::json::json_to_obj(data, json_str);

            auto init=[&](){
                 if (!this->ocr_resources.contains(data.target_key)){
                     auto ocr_resources=std::make_unique<rt::FaaServiceOcrResources>();
                     try{
                         rt::init::faa_ocr_init(*ocr_resources, resources_config_json["ocr_config"]);
                     }
                     catch (std::exception& e){
                         this->log.error("faa_ocr_init failed: {}", e.what());
                         return;
                     }
                     this->ocr_resources.emplace(data.target_key, std::move(ocr_resources));
                 }
            };

            init();
            std::cout<<""<<std::endl;
            switch (task::task_operation_map[data.task_type]){
                case task::RunWGC: {
                        const auto wgc_cap=std::make_unique<rt::FaaWgcCapResources>();
                        rt::init::faa_wgc_init(*wgc_cap, data.target_key, this->log);
                        operation::wgc_run(
                            this->frame_mutex,
                            this->is_running,data.target_key,
                            this->frames_map,*wgc_cap
                        );
                        break;
                }
                case task::GetImg:{
                        operation::get_img(
                        this->frame_mutex,data.target_key,
                         this->frames_map
                    );
                    break;
                }
                case task::GetOCR:{
                        FaaSerOcrResult result{};
                    //     operation::get_to_ocr(
                    //     *this->ocr_resources[data.target_key],
                    //     this->frame_mutex,data.target_key,
                    //      this->frames_map,result
                    // );
                    break;
                }
                case task::Win32_NoDet:{


                    for (const auto& info:data.input_list){

                        if (info.move_x!=-1 && info.move_y!=-1){
                            const auto is_click=info.is_click;
                            operation::set_input_win32_mouse(data.target_key,this->win32_inputs,{
                                    .mouse_x=info.move_x,
                                    .mouse_y=info.move_y,
                                    // .enable_smooth_move=info.enable_smooth_move,
                            },is_click);
                        }

                        if (!info.key_name.empty()){
                            operation::set_input_win32_key(data.target_key,this->win32_inputs,{
                                .delay_time = info.delay_time,
                                .key_name   =info.key_name.c_str(),
                            });
                        }

                    }
                    break;
                 }
                case task::NoTask:{
                    break;
                }

                default:
                    break;
            }

            if (data.task_type == "get") {

            }
            // if (data.task_type == "run"){
            //     sendto(this->runtime.server_socket,
            //        response.c_str(),
            //        static_cast<int32_t>(response.size()),
            //         0,
            //            reinterpret_cast<const sockaddr*>(&client_copy),
            //     sizeof(client_copy)
            //     );
            //     int count=0;
            //     while (is_running){
            //         auto frame = rt::FaaFrame{};
            //         get_frame(*resources.find(data.target_key)->second, frame);
            //         std::lock_guard lock(this->frame_mutex);
            //         if(!frames_map.contains(data.target_key)){
            //             frames_map.emplace(data.target_key, std::vector<cv::Mat>{});
            //         }
            //         frames_map[data.target_key].push_back(frame.frame);
            //         if (frames_map[data.target_key].size()>3){
            //             frames_map[data.target_key].erase(frames_map[data.target_key].begin());
            //         }
            //         count++;
            //     }
            // }
            // if (data.task_type=="get"){ auto
            //     frame = rt::FaaFrame{};
            //     std::lock_guard lock(this->frame_mutex);
            //
            //     if ( frames_map.contains(data.target_key) && !frames_map[data.target_key].empty()){
            //                   frame.frame=frames_map[data.target_key].back();
            //     }
            //     if (frame.frame.empty()){
            //         return;
            //     }
            //     FaaSerOcrResult result;
            //     ocr_proc_frame(*resources.find(data.target_key)->second, frame,result);
            //     auto all_res_str=result.toString();
            //     sendto(this->runtime.server_socket,
            //        all_res_str.c_str(),
            //        static_cast<int32_t>(all_res_str.size()),
            //         0,
            //              reinterpret_cast<const sockaddr*>(&client_copy),
            //         sizeof(client_copy)
            //     );
            //
            // }

        });


    }
    return 0;
}

int faa::service::FAAServiceUDP::run_cmd() {
    while (this->is_running.load()) {
        std::string cmd;
        std::cin>>cmd;
        if (cmd == "/stop") {
            this->log.info("stop");
            stop();
            break;
        }
    }
    return 0;
}

void faa::service::FAAServiceUDP::start() {
    loop_thread = std::thread(&FAAServiceUDP::run_loop, this);
    cmd_thread  = std::thread(&FAAServiceUDP::run_cmd, this);
}

void faa::service::FAAServiceUDP::stop() {
    is_running.store(false);   // 原子操作，安全退出
    if (runtime.server_socket != INVALID_SOCKET) {
        closesocket(runtime.server_socket);
        runtime.server_socket = INVALID_SOCKET;
    }
}

void faa::service::FAAServiceUDP::join() {
    if (loop_thread.joinable()) loop_thread.join();
    if (cmd_thread.joinable()) cmd_thread.join();
}


