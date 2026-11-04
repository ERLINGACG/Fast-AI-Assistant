//
// Created by HP on 2026/10/17.
//
#include "faa_cloud/microservices/general/service/faa_cloud_general.h"
using faa::cloud::microservices::general::FaaCloudGeneralServices;

void FaaCloudGeneralServices::setLogger(info::log::Ilog* logger) {
    this->logger = logger;
}

// TODO: 初始化线程池大小
void FaaCloudGeneralServices::setThreadPoolSize(int thread_pool_size) {
    // this->thread_pool = core::thread_pool::FAAThreadPool(thread_pool_size);
}


void FaaCloudGeneralServices::addAllThread(info::log::Ilog* log) {
    this->addCommand_loop(log);
    this->addCapThread(log);
    this->addNetworkThread(log);
}

void FaaCloudGeneralServices::addAllOper(info::log::Ilog* log) {
    this->addRequestOper(log);
    this->addWgcCaptureOper(log);
    this->addOcrOper(log);
    this->addYoloOper(log);
    this->addGetYoloModel(log);
    this->addSurfOper(log);
    this->addWin32InputOper(log);
    this->addHoughCircleOper();
}
