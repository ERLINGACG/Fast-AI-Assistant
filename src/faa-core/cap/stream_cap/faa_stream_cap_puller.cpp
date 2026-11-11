//
// Created by HP on 2026/11/9.
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
using faa::core::cap::stream::avErr;
using faa::core::cap::stream::wantListen;




bool FaaStreamCapture::openPuller(
    const std::string& stream_url,
    const StreamProto& proto,
    const DeCodeOptParam& opt
) {
    auto& P = impl_ptr->pull;
    P.close();

    const std::string url = buildUrl(stream_url, proto);

    // ===== 1. 协议选项 =====
    AVDictionary* dict = nullptr;
    if (wantListen(proto)) {
        av_dict_set(&dict, "listen", "1", 0);
    }
    if (std::holds_alternative<Tcp>(proto)) {
        av_dict_set(&dict, "tcp_nodelay", "1", 0);
    }
    if (std::holds_alternative<Udp>(proto)) {
        av_dict_set(&dict, "fifo_size",       "1000000", 0);
        av_dict_set(&dict, "overrun_nonfatal", "1",       0);
    }

    // ===== 2. 输入上下文 + 中断回调（必须在 open_input 之前挂） =====
    P.inCtx = avformat_alloc_context();
    if (!P.inCtx) {
        impl_ptr->setError("alloc inCtx failed");
        av_dict_free(&dict);
        return false;
    }
    P.inCtx->interrupt_callback.callback = &FaaStreamCapImpl::FaaStreamCapPull::interruptCb;
    P.inCtx->interrupt_callback.opaque   = &P;
    P.running = true;   // 中断回调依赖这个；listen 模式下 open 会阻塞，必须提前置 true

    // ===== 3. 打开输入 =====
    int ret = avformat_open_input(&P.inCtx, url.c_str(), nullptr, &dict);
    av_dict_free(&dict);
    if (ret < 0) {
        impl_ptr->setError("avformat_open_input [" + url + "]: " + avErr(ret));
        P.close();
        return false;
    }

    // ===== 4. 探测流信息 =====
    ret = avformat_find_stream_info(P.inCtx, nullptr);
    if (ret < 0) {
        impl_ptr->setError("find_stream_info: " + avErr(ret));
        P.close();
        return false;
    }

    // ===== 5. 找视频流 =====
    P.streamIdx = av_find_best_stream(P.inCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (P.streamIdx < 0) {
        impl_ptr->setError("no video stream");
        P.close();
        return false;
    }

    // ===== 6. 解码器 =====
    const AVCodec* dec = nullptr;
    if (!opt.codec.empty()) {
        dec = avcodec_find_decoder_by_name(opt.codec.c_str());
        if (!dec) {
            impl_ptr->setError("decoder not found: " + opt.codec);
            P.close();
            return false;
        }
    } else {
        dec = avcodec_find_decoder(P.inCtx->streams[P.streamIdx]->codecpar->codec_id);
        if (!dec) {
            impl_ptr->setError("no decoder for codec_id");
            P.close();
            return false;
        }
    }

    P.decCtx = avcodec_alloc_context3(dec);
    if (!P.decCtx) {
        impl_ptr->setError("alloc decCtx failed");
        P.close();
        return false;
    }
    ret = avcodec_parameters_to_context(P.decCtx, P.inCtx->streams[P.streamIdx]->codecpar);
    if (ret < 0) {
        impl_ptr->setError("parameters_to_context: " + avErr(ret));
        P.close();
        return false;
    }
    if (opt.threads > 0) P.decCtx->thread_count = opt.threads;
    P.decCtx->pkt_timebase = P.inCtx->streams[P.streamIdx]->time_base;
    ret = avcodec_open2(P.decCtx, dec, nullptr);
    if (ret < 0) {
        impl_ptr->setError("codec_open2: " + avErr(ret));
        P.close();
        return false;
    }

    // ===== 7. 缓冲 =====
    P.decFrame = av_frame_alloc();
    P.pkt      = av_packet_alloc();
    if (!P.decFrame || !P.pkt) {
        impl_ptr->setError("alloc frame/pkt failed");
        P.close();
        return false;
    }

    auto* st = P.inCtx->streams[P.streamIdx];
    std::printf("decoder: %s (%dx%d, codec=%s)\n",
                dec->name, st->codecpar->width, st->codecpar->height,
                avcodec_get_name(st->codecpar->codec_id));

    return true;
}
bool FaaStreamCapture::pull(frame::FaaMat& mat) const
{
    auto& P = impl_ptr->pull;
    if (!P.running) {
        impl_ptr->setError("puller not open");
        return false;
    }

    while (true) {
        // ===== 1. 读包 =====
        int ret = av_read_frame(P.inCtx, P.pkt);
        if (ret == AVERROR_EXIT) {
            impl_ptr->setError("interrupted");
            return false;
        }
        if (ret < 0) {
            impl_ptr->setError("read_frame: " + avErr(ret));
            return false;
        }

        if (P.pkt->stream_index != P.streamIdx) {
            av_packet_unref(P.pkt);
            continue;   // 非视频流，丢弃
        }

        // ===== 2. 送解码器 =====
        ret = avcodec_send_packet(P.decCtx, P.pkt);
        av_packet_unref(P.pkt);
        if (ret < 0) continue;   // 坏包，跳过

        // ===== 3. 收解码帧 =====
        ret = avcodec_receive_frame(P.decCtx, P.decFrame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            continue;   // 需要更多包，继续读
        }
        if (ret < 0) {
            impl_ptr->setError("receive_frame: " + avErr(ret));
            return false;
        }

        // ===== 4. 首次拿到帧，按实际尺寸/像素格式建 sws =====
        if (!P.sws) {
            P.outW = P.decFrame->width;
            P.outH = P.decFrame->height;
            P.sws = sws_getContext(P.outW, P.outH, (AVPixelFormat)P.decFrame->format,
                                   P.outW, P.outH, AV_PIX_FMT_BGR24,
                                   SWS_BILINEAR, nullptr, nullptr, nullptr);
            if (!P.sws) {
                impl_ptr->setError("sws_getContext failed");
                return false;
            }
            std::printf("pull: %dx%d, src_fmt=%d\n",
                        P.outW, P.outH, P.decFrame->format);
        }

        // ===== 5. 转换到 BGR 输出 =====
        auto& m = mat.impl()->mat;
        if (m.cols != P.outW || m.rows != P.outH || m.type() != CV_8UC3) {
            m.create(P.outH, P.outW, CV_8UC3);
        }

        std::uint8_t* dst[1]       = { m.data };
        int           dstStride[1] = { static_cast<int>(m.step) };
        sws_scale(P.sws, P.decFrame->data, P.decFrame->linesize,
                  0, P.outH, dst, dstStride);

        av_frame_unref(P.decFrame);
        return true;
    }
}
