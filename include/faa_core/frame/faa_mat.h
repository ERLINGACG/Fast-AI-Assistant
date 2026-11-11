//
// Created by HP on 2026/10/20.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_FRAME_MAT_H
#define FASTAIASSISTANT_ROOT_FAA_FRAME_MAT_H
#include <memory>
#include <string>
#include <vector>
#include "faa_core/faa_export.h"
namespace faa::core::frame {

    struct FaaMatImpl;

    struct FAA_CPP_EXPORT FaaMat
    {
        std::unique_ptr<FaaMatImpl> impl_;

        FaaMat();
        ~FaaMat();
        FaaMat(FaaMat&&) noexcept;
        FaaMat(const FaaMat&) = delete;
        FaaMat& operator=(FaaMat&&) noexcept;
        FaaMat& operator=(const FaaMat&) = delete;

        [[nodiscard]] auto& impl() const {
            return impl_;
        }

        [[nodiscard]] bool empty() const;
        [[nodiscard]] bool read (const std::string& filename) const;
        [[nodiscard]] bool write(const std::string& filename) const;

        [[nodiscard]] bool show(int delay_ms = 0,const std::string& name = "mat") const;
        [[nodiscard]] int64_t total() const; // 总元素数
        [[nodiscard]] int64_t row() const;  // 行数
        [[nodiscard]] int64_t col() const;  // 列数
        [[nodiscard]] int dims() const;
        [[nodiscard]] std::vector<int64_t> shape() const;

        void resize(int w, int h);
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_FRAME_MAT_H