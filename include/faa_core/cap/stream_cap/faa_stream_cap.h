//
// Created by HP on 2026/11/8.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_H
#define FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_H
#include <memory>
#include <string>

#include "faa_stream_cap_enum.h"
#include "faa_stream_cap_param.h"
#include "faa_core/faa_export.h"
#include "faa_core/frame/faa_mat.h"
#include "info/ilog/ilog.h"

namespace  faa::core::cap::stream {

    struct FaaStreamCapImpl;

    class FAA_CPP_EXPORT FaaStreamCapture {

        std::unique_ptr<FaaStreamCapImpl> impl_ptr;
        info::log::Ilog* logger = nullptr;

    public:
        FaaStreamCapture();
        ~FaaStreamCapture();

        FaaStreamCapture(FaaStreamCapture&&) noexcept;        // 只声明
        FaaStreamCapture& operator=(FaaStreamCapture&&) noexcept;

        FaaStreamCapture(const FaaStreamCapture&) = delete;   // delete 可以留头文件
        FaaStreamCapture& operator=(const FaaStreamCapture&) = delete;

        void setLogger(info::log::Ilog* logger) { this->logger = logger;}

        bool openPuller (
            const std::string& stream_url,
            const proto::StreamProto& proto,
            const DeCodeOptParam& opt
            );



        bool openPusher(
            const std::string& stream_url,
            const proto::StreamProto& proto,
            const EndCodeOptParam& opt
        );

        void closePusher();


        bool push (frame::FaaMat& mat);
        bool pull (frame::FaaMat& mat) const;

        std::string lastError() const;
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_STREAM_CAP_H