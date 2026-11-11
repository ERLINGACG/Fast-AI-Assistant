//
// Created by HP on 2026/11/8.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_ENUM_H
#define FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_ENUM_H
#include <string>
#include <variant>

namespace faa::core::cap::stream::proto {

    struct Tcp {
        int  connectTimeoutMs = 5000;
        bool listen           = false;   // 服务端模式
        std::string format    = "mpegts"; // 推流封装
    };

    struct Udp {
        int  connectTimeoutMs = 5000;
        int  bufferSize       = 0;        // 0 = FFmpeg 默认
        bool listen           = false;
        std::string format    = "mpegts";
    };

    struct Rtsp {
        enum class Transport { Auto, Tcp, Udp };
        Transport   transport        = Transport::Auto;
        int         connectTimeoutMs = 5000;
        int         readTimeoutMs    = 5000;
        // RTSP 推流 format 固定 "rtsp"，不需要字段
    };


    using StreamProto = std::variant<Tcp, Udp, Rtsp>;

}
#endif //FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_ENUM_H