//
// Created by HP on 2026/11/8.
//
#include "faa_core/cap/stream_cap/faa_stream_cap_utils.h"
extern "C" {
    #include <libavutil/error.h>
}

std::string faa::core::cap::stream::buildUrl(const std::string& endpoint, const proto::StreamProto& p) {
    auto hasScheme = endpoint.find("://") != std::string::npos;
    return std::visit([&](auto&&) -> std::string {
        using T = std::decay_t<decltype(std::declval<decltype(p)>)>;
        // 更简单：直接判断 endpoint 是否已带 scheme
        if (hasScheme) return endpoint;
        std::string scheme;
        if (std::holds_alternative<proto::Tcp>(p))  scheme = "tcp://";
        else if (std::holds_alternative<proto::Udp>(p))  scheme = "udp://";
        else scheme = "rtsp://";
        return scheme + endpoint;
    }, p);
}

std::string faa::core::cap::stream::pusherFormat(const proto::StreamProto& p)
{
    return std::visit([]<typename T0>(T0&& v) -> std::string {
      using T = std::decay_t<T0>;
      if constexpr (std::is_same_v<T, proto::Tcp>)  return v.format;
      if constexpr (std::is_same_v<T, proto::Udp>)  return v.format;
      if constexpr (std::is_same_v<T, proto::Rtsp>) return "rtsp";
      return "";
  }, p);
}

std::string faa::core::cap::stream::avErr(int ret) {
    char buf[AV_ERROR_MAX_STRING_SIZE] = {0};
    av_strerror(ret, buf, sizeof(buf));
    return std::string(buf);
}

bool faa::core::cap::stream::wantListen(const proto::StreamProto& p)
{
    return std::visit([]<typename T0>(T0&& v) -> bool {
        using T = std::decay_t<T0>;
        if constexpr (std::is_same_v<T, proto::Tcp> || std::is_same_v<T, proto::Udp>)
            return v.listen;
        return false;
    }, p);
}
