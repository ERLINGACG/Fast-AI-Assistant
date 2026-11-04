//
// Created by HP on 2026/10/20.
//
#include "faa_core/frame/private/faa_mat_impi.h"

#include <opencv2/imgcodecs.hpp>

#include "faa_core/frame/faa_mat.h"
using faa::core::frame::FaaMat;
using faa::core::frame::FaaMatImpl;

FaaMat::FaaMat() {
    impl_ = std::make_unique<FaaMatImpl>();
}

FaaMat::~FaaMat() = default;

FaaMat::FaaMat(FaaMat&& other) noexcept = default;

FaaMat& FaaMat::operator=(FaaMat&&) noexcept = default;


bool FaaMat::empty() const{
    return impl_->mat.empty();
}

bool FaaMat::read(const std::string& filename) const{
    impl_->mat = cv::imread(filename);
    return !impl_->mat.empty();
}

bool FaaMat::write(const std::string& filename) const{
    return cv::imwrite(filename,impl_->mat);
}



int64_t FaaMat::total() const
{
    return static_cast<int64_t>(impl_->mat.total());
}

int64_t FaaMat::row() const
{
    return impl_->mat.rows;
}

int64_t FaaMat::col() const
{
    return impl_->mat.cols;
}

int FaaMat::dims() const {
    return impl_->mat.dims;
}

std::vector<int64_t> FaaMat::shape() const
{
    std::vector<int64_t> shape;
    for (int i = 0; i < impl_->mat.dims; i++){
        shape.push_back(impl_->mat.size[i]);
    }
    return shape;
}
