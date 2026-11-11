//
// Created by HP on 2026/11/8.
//
#include "faa_core/cap/stream_cap/faa_stream_cap.h"
#include "faa_core/cap/stream_cap/private/faa_stream_cap_impl.h"
#include "faa_core/cap/stream_cap/faa_stream_cap_utils.h"
#include <opencv2/imgproc.hpp>
#include "faa_core/frame/private/faa_mat_impi.h"
using faa::core::cap::stream::FaaStreamCapture;
using faa::core::cap::stream::proto::StreamProto;
using faa::core::cap::stream::proto::Tcp;
using faa::core::cap::stream::proto::Rtsp;
using faa::core::cap::stream::proto::Udp;
using faa::core::cap::stream::buildUrl;
using faa::core::cap::stream::pusherFormat;


void fillAvOptions(const StreamProto& p, AVDictionary** d) {
    std::visit([&](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, Tcp> || std::is_same_v<T, Udp>) {
            av_dict_set(d, "timeout", std::to_string(v.connectTimeoutMs * 1000).c_str(), 0);
            if (v.listen) av_dict_set(d, "listen", "1", 0);
        }
        if constexpr (std::is_same_v<T, Udp>) {
            if (v.bufferSize > 0)
                av_dict_set(d, "buffer_size", std::to_string(v.bufferSize).c_str(), 0);
        }
        if constexpr (std::is_same_v<T, Rtsp>) {
            av_dict_set(d, "stimeout", std::to_string(v.readTimeoutMs * 1000).c_str(), 0);
            if (v.transport == Rtsp::Transport::Tcp)
                av_dict_set(d, "rtsp_transport", "tcp", 0);
            else if (v.transport == Rtsp::Transport::Udp)
                av_dict_set(d, "rtsp_transport", "udp", 0);
        }
    }, p);
}

FaaStreamCapture::FaaStreamCapture() {
    this->impl_ptr = std::make_unique<FaaStreamCapImpl>();
}

FaaStreamCapture::~FaaStreamCapture() = default;
FaaStreamCapture::FaaStreamCapture(FaaStreamCapture&&) noexcept = default;
FaaStreamCapture& FaaStreamCapture::operator=(FaaStreamCapture&&) noexcept = default;




std::string FaaStreamCapture::lastError() const
{
    return impl_ptr->getError();
}
