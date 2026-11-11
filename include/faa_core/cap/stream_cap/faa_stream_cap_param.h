#ifndef FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_PARAM_H
#define FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_PARAM_H
#include <map>
#include <string>

namespace  faa::core::cap::stream {


    struct EndCodeOptParam {
        std::string codec;

        // 必填：视频尺寸与帧率
        int width       = 0;
        int height      = 0;
        int fps         = 30;

        // 码率控制（kbps），CBR
        int bitrateKbps = 4000;

        // 0 = fps * 2
        int gopSize     = 0;

        // 0 = FFmpeg 自动
        int threads     = 0;

        // 编码器通用调优（libx264/nvenc 都认，值不同）
        std::string preset = "veryfast";
        std::string tune   = "zerolatency";

        // 逃生舱：任意编码器私有选项，直传 avcodec_open2
        // 例：{"crf","23"}、{"rc","vbr"}、{"delay","0"}
        std::map<std::string, std::string> extra;
    };

    struct DeCodeOptParam {
        // 解码器名：空 = 按流里的 codec_id 自动选
        // 常用硬解：h264_cuvid / hevc_cuvid / h264_qsv / h264_d3d11va
        std::string codec;

        // 解码线程数：0 = FFmpeg 自动（通常 = CPU 核心数）
        int threads = 0;

        // 低延迟模式：减少解码器内部缓冲，代价是首帧可能花屏
        // 对应 AV_CODEC_FLAG_LOW_DELAY
        bool lowDelay = false;

        // 跳过环路滤波：解码快一点，画质略降（几乎看不出）
        // 对应 AV_CODEC_FLAG2_FAST
        bool skipLoopFilter = false;

        // 逃生舱：任意解码器私有选项，直传 avcodec_open2
        // 例：{"gpu","0"}、{"surfaces","32"}（cuvid 专属）
        std::map<std::string, std::string> extra;
    };


}
#endif //FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_PARAM_H