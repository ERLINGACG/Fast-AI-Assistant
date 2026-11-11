//
// Created by HP on 2026/11/8.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_UTILS_H
#define FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_UTILS_H
#include "faa_stream_cap_enum.h"

namespace  faa::core::cap::stream {
    std::string buildUrl(const std::string& endpoint, const proto::StreamProto& p);
    std::string pusherFormat(const proto::StreamProto& p);
    std::string avErr(int ret);
    bool wantListen(const proto::StreamProto& p);

}
#endif //FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_UTILS_H