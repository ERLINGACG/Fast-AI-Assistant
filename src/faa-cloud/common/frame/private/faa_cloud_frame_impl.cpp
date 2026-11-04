//
// Created by HP on 2026/10/22.
//
#include "faa_cloud/common/frame/faa_cloud_frame.h"
#include "faa_cloud/common/frame/private/faa_cloud_frame_impl.h"
#include "faa_core/frame/private/faa_mat_impi.h"
using faa::cloud::frame::Frames;
using faa::cloud::frame::FramesImpl;

Frames::Frames() {
    impi_ptr = std::make_unique<FramesImpl>();
}

Frames::~Frames() = default;
Frames::Frames(Frames&& other) noexcept = default;
Frames& Frames::operator=(Frames&& other) noexcept = default;


void Frames::keep(const int32_t max) const{
    impi_ptr->keep(max);
}

void Frames::push(const core::frame::FaaMat& mat) const{
    impi_ptr->push(mat.impl()->mat);
}

void Frames::peek(core::frame::FaaMat& mat) const{
    impi_ptr->peek(mat.impl()->mat);
}
