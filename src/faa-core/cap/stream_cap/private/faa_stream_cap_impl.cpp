//
// Created by HP on 2026/11/8.
//
#include "faa_core/cap/stream_cap/private/faa_stream_cap_impl.h"

#include <mutex>
#include <string>
using faa::core::cap::stream::FaaStreamCapImpl;



FaaStreamCapImpl::FaaStreamCapImpl() {
    avformat_network_init();
    pull.owner = this;
    push.owner = this;
}

void FaaStreamCapImpl::setError(std::string msg) {
    std::lock_guard lk(errMtx);
    lastError = std::move(msg);
}

std::string FaaStreamCapImpl::getError() const {
    std::lock_guard lk(errMtx);
    return lastError;
}

void FaaStreamCapImpl::FaaStreamCapPull::close() {
    running = false;
    if (sws)      { sws_freeContext(sws); sws = nullptr; }
    if (decFrame) { av_frame_free(&decFrame); }
    if (pkt)      { av_packet_free(&pkt); }
    if (decCtx)   { avcodec_free_context(&decCtx); }
    if (inCtx)    { avformat_close_input(&inCtx); }
    streamIdx = -1;
    outW = outH = 0;
}

void FaaStreamCapImpl::FaaStreamCapPush::close() {

    if (encCtx && outCtx && outStrm && pkt) {
        avcodec_send_frame(encCtx, nullptr);
        while (true) {
            int ret = avcodec_receive_packet(encCtx, pkt);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
            if (ret < 0) break;
            av_packet_rescale_ts(pkt, encCtx->time_base, outStrm->time_base);
            pkt->stream_index = outStrm->index;
            av_interleaved_write_frame(outCtx, pkt);
            av_packet_unref(pkt);
        }
    }

    // 原有释放逻辑
    if (sws)      { sws_freeContext(sws); sws = nullptr; }
    if (encFrame) { av_frame_free(&encFrame); }
    if (pkt)      { av_packet_free(&pkt); }
    if (encCtx)   { avcodec_free_context(&encCtx); }
    if (outCtx) {
        if (outCtx->pb && !(outCtx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&outCtx->pb);
        avformat_free_context(outCtx);
        outCtx = nullptr;
    }
    outStrm = nullptr;
    w = h = 0;
    frameIdx = 0;
}
