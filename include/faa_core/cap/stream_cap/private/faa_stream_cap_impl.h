//
// Created by HP on 2026/11/8.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_IMPL_H
#define FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_IMPL_H
#include <atomic>
#include <mutex>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/opt.h>
}
namespace faa::core::cap::stream {


    struct FaaStreamCapImpl {

        struct FaaStreamCapPush {

            FaaStreamCapImpl* owner;
            AVFormatContext*  outCtx      = nullptr;   // avformat_alloc_output_context2 产生
            AVCodecContext*   encCtx      = nullptr;   // 编码器上下文
            AVStream*         outStrm     = nullptr;   // 输出流
            AVFrame*          encFrame    = nullptr;   // 编码输入帧（YUV）
            AVPacket*         pkt         = nullptr;   // 编码输出包
            SwsContext*       sws         = nullptr;   // BGR -> YUV 转换
            int               w           = 0;         // 编码宽
            int               h           = 0;         // 编码高
            int               fps         = 30;
            int               bitrateKbps = 4000;
            int64_t           frameIdx    = 0;         // pts 计数
            int last_src_w = 0;
            int last_src_h = 0;

            ~FaaStreamCapPush() { close(); }
            void close();

        } push;

        struct FaaStreamCapPull {

            FaaStreamCapImpl* owner;
            AVFormatContext*  inCtx    = nullptr;
            AVCodecContext*   decCtx   = nullptr;
            AVFrame*          decFrame = nullptr;
            AVPacket*         pkt      = nullptr;
            SwsContext*       sws      = nullptr;
            int               streamIdx = -1;
            int               outW = 0, outH = 0;
            std::atomic_bool running{false};

            ~FaaStreamCapPull() { close(); }
            void close();

            static int interruptCb(void* opaque) {
                auto* self = static_cast<FaaStreamCapPull*>(opaque);
                return self->running.load() ? 0 : 1;
            }

        } pull;

        // void init();
        // void cleanup();
        FaaStreamCapImpl();
        ~FaaStreamCapImpl() = default;

        mutable std::mutex errMtx;
        std::string        lastError;

        void setError(std::string msg);
        std::string getError() const;
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_IMPL_H