//
// Created by HP on 2026/8/31.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_CLOUD_FRAME_H
#define FASTAIASSISTANT_ROOT_FAA_CLOUD_FRAME_H
#include <memory>

#include "faa_core/frame/faa_mat.h"
#include "faa_core/faa_export.h"

namespace faa::cloud::frame{

    struct FramesImpl;

    struct FAA_CPP_EXPORT Frames{


        std::unique_ptr<FramesImpl> impi_ptr;

        Frames();
        ~Frames();
        Frames(Frames&&) noexcept;
        Frames(const Frames&) = delete;
        Frames& operator=(Frames&&) noexcept;
        Frames& operator=(const Frames&) = delete;


        [[nodiscard]] auto& impl() const {
            return impi_ptr;
        }

        void peek(core::frame::FaaMat& mat) const;

        void keep(int32_t max) const;

        void push(const core::frame::FaaMat& mat) const;

    };

}
#endif //FASTAIASSISTANT_ROOT_FAA_CLOUD_FRAME_H
