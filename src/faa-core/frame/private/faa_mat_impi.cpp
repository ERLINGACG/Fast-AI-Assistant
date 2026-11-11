//
// Created by HP on 2026/10/20.
//
#include "faa_core/frame/private/faa_mat_impi.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

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

bool FaaMat::show(const int delay_ms, const std::string& name) const {
    if (impl_->mat.empty()) {
        return true;
    }

    // 静态标志位：确保整个程序生命周期内只执行一次窗口创建
    static bool is_window_initialized = false;

    if (!is_window_initialized) {
        // 创建可自由缩放、且保持长宽比的窗口
        cv::namedWindow(name, cv::WINDOW_NORMAL);

        // 第一次显示时，将窗口大小严格设置为视频的原始分辨率 (1:1)
        cv::resizeWindow(name,1280,720);

        is_window_initialized = true;
    }

    cv::imshow(name, impl_->mat);

    int key = cv::waitKey(delay_ms);
    if (key == 27) { // 27 是 ESC 键
        cv::destroyWindow(name);
        is_window_initialized = false; // 如果销毁了，下次调用需要重新创建
        return false;
    }
    return true;
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

void FaaMat::resize(int w, int h)
{
}


