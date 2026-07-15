//
// Created by HP on 2026/6/29.
//
#include "faa_service/faa_service.h"
#include <ws2tcpip.h>
#include <opencv2/highgui.hpp>

#include "faa-core/faa_data.h"
#include "faa-core/win_utils/faa_win_utils.h"
#include "faa-utlis/fs_json_utils.h"

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
            }else{
                this->log.error("recvfrom failed: {}", error);
            }
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

            auto data=core::data::RequestData{};
            utils::json::json_to_obj(data, json_str);
            this->log.info("target_key: {}", data.target_key);
            std::lock_guard lock(this->window_capture_mutex);
            auto it = window_capture.find(data.target_key);
            if (it == window_capture.end()) {
               this->log.debug("the windows is first access to {}", data.target_key);
                auto d3d_ctx=core::cap::wgc::D3DDeviceContext();
                auto wgc_rt=core::cap::wgc::WGCRunTimeContext();

                auto wgc_cap=core::cap::wgc::FAAWindowCapture(data.target_key, this->log).
                      initD3d11(d3d_ctx).
                      initWgcRtCtx(wgc_rt).
                      startSession();

                if (!wgc_cap.is_init()){
                    this->log.error("the windows not found to {}", data.target_key);
                    return;
                }
                this->window_capture.emplace(data.target_key, std::move(wgc_cap));
            }

            auto& cap = this->window_capture.at(data.target_key);
            this->log.debug("the windows not first access to {}", data.target_key);
            this->window_capture_mutex.unlock();
            int count=0;
            cv::Mat mat;
            auto texture_data=core::cap::wgc::WgcTextureData();
            while (mat.empty()){
                cap.getFrame(texture_data, mat);
                this->log.debug("getFrame failed, try again, count: {}", count++);
            }


            if (mat.empty()){
                this->log.error("getFrame failed");
                return;
            }
            this->log.info("getFrame success");
            this->log.info("mat size: {}, {}", mat.rows, mat.cols);
            std::string filename = "capture_" + std::to_string(std::time(nullptr)) + ".png";
            if (cv::imwrite(filename, mat)) {
                this->log.info("Frame saved to {}", filename);
            } else {
                this->log.error("Failed to save frame");
            }


            std::string response="success";
            sendto(this->runtime.server_socket,
                   response.c_str(),
                   static_cast<int32_t>(response.size()),
                    0,
                       reinterpret_cast<const sockaddr*>(&client_copy),
                sizeof(client_copy)
           );


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


