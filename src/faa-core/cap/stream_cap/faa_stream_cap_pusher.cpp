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



bool FaaStreamCapture::openPusher(
    const std::string& stream_url,
    const StreamProto& proto,
    const EndCodeOptParam& opt
) {
    auto& Q = impl_ptr->push;
    Q.close();

    // ===== 1. 参数校验 =====
    if (opt.width <= 0 || opt.height <= 0 || opt.fps <= 0) {
        impl_ptr->setError("invalid encoder params: width=" + std::to_string(opt.width)
                         + " height=" + std::to_string(opt.height)
                         + " fps=" + std::to_string(opt.fps));
        return false;
    }

    const std::string url = buildUrl(stream_url, proto);
    const std::string fmt = pusherFormat(proto);

    // ===== 2. 输出上下文 =====
    int ret = avformat_alloc_output_context2(&Q.outCtx, nullptr, fmt.c_str(), url.c_str());
    if (ret < 0 || !Q.outCtx) {
        impl_ptr->setError("alloc_output_context2: " + avErr(ret));
        return false;
    }

    // ===== 3. 编码器选择 =====
    const AVCodec* codec = nullptr;
    if (!opt.codec.empty()) {
        codec = avcodec_find_encoder_by_name(opt.codec.c_str());
        if (!codec) {
            impl_ptr->setError("encoder not found: " + opt.codec);
            Q.close();
            return false;
        }
    } else {
        for (const char* n : { "libx264", "libopenh264", "mpeg4" }) {
            codec = avcodec_find_encoder_by_name(n);
            if (codec) break;
        }
        if (!codec) {
            impl_ptr->setError("no fallback encoder available");
            Q.close();
            return false;
        }
    }

    // ===== 4. 编码器上下文 =====
    Q.encCtx = avcodec_alloc_context3(codec);
    if (!Q.encCtx) {
        impl_ptr->setError("alloc encCtx failed");
        Q.close();
        return false;
    }
    Q.encCtx->width        = opt.width;
    Q.encCtx->height       = opt.height;
    Q.encCtx->time_base    = { 1, opt.fps };
    Q.encCtx->framerate    = { opt.fps, 1 };
    Q.encCtx->pix_fmt      = AV_PIX_FMT_YUV420P;
    Q.encCtx->bit_rate     = static_cast<int64_t>(opt.bitrateKbps) * 1000;
    Q.encCtx->gop_size     = opt.gopSize > 0 ? opt.gopSize : opt.fps * 2;
    Q.encCtx->max_b_frames = 0;
    if (opt.threads > 0) Q.encCtx->thread_count = opt.threads;
    if (Q.outCtx->oformat->flags & AVFMT_GLOBALHEADER)
        Q.encCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    // ===== 5. 编码器选项 =====
    AVDictionary* encOpts = nullptr;
    if (!opt.preset.empty()) av_dict_set(&encOpts, "preset", opt.preset.c_str(), 0);
    if (!opt.tune.empty())   av_dict_set(&encOpts, "tune",   opt.tune.c_str(),   0);
    for (const auto& [k, v] : opt.extra)
        av_dict_set(&encOpts, k.c_str(), v.c_str(), 0);

    ret = avcodec_open2(Q.encCtx, codec, &encOpts);
    if (ret < 0) {
        std::string err = "codec_open2 [" + std::string(codec->name) + "]: " + avErr(ret);
        // 把 FFmpeg 没消费的选项打出来，方便定位是哪个 key 不合法
        if (encOpts) {
            AVDictionaryEntry* t = nullptr;
            err += " | unknown opts:";
            while ((t = av_dict_get(encOpts, "", t, AV_DICT_IGNORE_SUFFIX)))
                err += " " + std::string(t->key) + "=" + t->value;
        }
        impl_ptr->setError(err);
        av_dict_free(&encOpts);
        Q.close();
        return false;
    }
    av_dict_free(&encOpts);

    std::printf("encoder: %s (%dx%d @ %dfps, %dkbps)\n",
                codec->name, opt.width, opt.height, opt.fps, opt.bitrateKbps);

    // ===== 6. 输出流 =====
    Q.outStrm = avformat_new_stream(Q.outCtx, nullptr);
    if (!Q.outStrm) {
        impl_ptr->setError("new_stream failed");
        Q.close();
        return false;
    }
    ret = avcodec_parameters_from_context(Q.outStrm->codecpar, Q.encCtx);
    if (ret < 0) {
        impl_ptr->setError("parameters_from_context: " + avErr(ret));
        Q.close();
        return false;
    }
    Q.outStrm->time_base = Q.encCtx->time_base;

    // ===== 7. 打开 IO（listen / tcp_nodelay 从这里传给 FFmpeg） =====
    if (!(Q.outCtx->oformat->flags & AVFMT_NOFILE)) {
        AVDictionary* ioOpts = nullptr;
        if (wantListen(proto)) {
            av_dict_set(&ioOpts, "listen", "1", 0);
        }
        if (std::holds_alternative<proto::Tcp>(proto)) {
            av_dict_set(&ioOpts, "tcp_nodelay", "1", 0);
        }
        if (std::holds_alternative<proto::Udp>(proto)) {

            av_dict_set(&ioOpts, "pkt_size", "1316", 0);          // 7*188，避免 IP 分片
            av_dict_set(&ioOpts, "buffer_size", "8388608", 0);    // 发送缓冲 8MB
        }

        ret = avio_open2(&Q.outCtx->pb, url.c_str(), AVIO_FLAG_WRITE, nullptr, &ioOpts);
        av_dict_free(&ioOpts);
        if (ret < 0) {
            impl_ptr->setError("avio_open2 [" + url + "]: " + avErr(ret));
            Q.close();
            return false;
        }
    }

    // ===== 8. 写头（listen 模式下会阻塞到客户端连入） =====
    ret = avformat_write_header(Q.outCtx, nullptr);
    if (ret < 0) {
        impl_ptr->setError("write_header: " + avErr(ret));
        Q.close();
        return false;
    }

    // ===== 9. 编码输入帧 =====
    Q.encFrame = av_frame_alloc();
    if (!Q.encFrame) {
        impl_ptr->setError("av_frame_alloc failed");
        Q.close();
        return false;
    }
    Q.encFrame->format = Q.encCtx->pix_fmt;
    Q.encFrame->width  = Q.encCtx->width;
    Q.encFrame->height = Q.encCtx->height;
    ret = av_frame_get_buffer(Q.encFrame, 0);
    if (ret < 0) {
        impl_ptr->setError("frame_get_buffer: " + avErr(ret));
        Q.close();
        return false;
    }

    // ===== 10. 像素格式转换 BGR24 -> YUV420P =====
    Q.sws = sws_getContext(opt.width, opt.height, AV_PIX_FMT_BGR24,
                           opt.width, opt.height, AV_PIX_FMT_YUV420P,
                           SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!Q.sws) {
        impl_ptr->setError("sws_getContext failed");
        Q.close();
        return false;
    }

    Q.pkt = av_packet_alloc();
    if (!Q.pkt) {
        impl_ptr->setError("av_packet_alloc failed");
        Q.close();
        return false;
    }

    Q.w = opt.width;
    Q.h = opt.height;
    Q.fps = opt.fps;
    Q.bitrateKbps = opt.bitrateKbps;
    Q.frameIdx = 0;
    return true;
}

bool FaaStreamCapture::push(frame::FaaMat& mat)
{

    auto& Q = impl_ptr->push;
    if (!Q.outCtx || !Q.encCtx) { impl_ptr->setError("pusher not open"); return false; }

    const auto& m = mat.impl()->mat;
    if (m.empty() || m.type() != CV_8UC3) { impl_ptr->setError("expect CV_8UC3"); return false; }

    cv::Mat scaled;
    const cv::Mat* src = &m;
    if (m.cols != Q.w || m.rows != Q.h) {
        cv::resize(m, scaled, cv::Size(Q.w, Q.h), 0, 0, cv::INTER_AREA);
        src = &scaled;
    }


    av_frame_make_writable(Q.encFrame);
    const std::uint8_t* srcData[1]   = { src->data };
    int                 srcStride[1] = { static_cast<int>(src->step) };
    sws_scale(Q.sws, srcData, srcStride, 0, Q.h, Q.encFrame->data, Q.encFrame->linesize);

    Q.encFrame->pts = Q.frameIdx++;
    int ret = avcodec_send_frame(Q.encCtx, Q.encFrame);
    if (ret < 0) { impl_ptr->setError("send_frame " + std::to_string(ret)); return false; }

    while (true) {
        ret = avcodec_receive_packet(Q.encCtx, Q.pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) return true;
        if (ret < 0) { impl_ptr->setError("receive_packet " + std::to_string(ret)); return false; }

        av_packet_rescale_ts(Q.pkt, Q.encCtx->time_base, Q.outStrm->time_base);
        Q.pkt->stream_index = Q.outStrm->index;
        ret = av_interleaved_write_frame(Q.outCtx, Q.pkt);
        av_packet_unref(Q.pkt);
        if (ret < 0) { impl_ptr->setError("write_frame " + std::to_string(ret)); return false; }
    }
}

void FaaStreamCapture::closePusher() {
    this->impl_ptr->push.close();
}