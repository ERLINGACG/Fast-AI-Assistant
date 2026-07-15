//
// Created by HP on 2026/6/29.
//

#include "faa_service/faa_service.h"
using faa::service::FAAServiceUDP;

int main(int argc, char** argv) {
    SetConsoleOutputCP(CP_UTF8);
    auto service=FAAServiceUDP([&](){
        return faa::service::net_work::NetWorkConfig{
             "127.0.0.1",
             8080,
             1024
        };
    }, info::log::Ilog([&](){
        return info::log::IlogInit{
        };
    }));

    service.init();

    service.start();
    service.join();

    return 0;
}