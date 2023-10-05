#pragma once

#include <cstdint>

namespace maseya::vkbase {
class StbImage {
private:
    StbImage() = default;

public:
    StbImage(int width, int height, int channels, unsigned char* pixels);
    StbImage(const char* path);

    StbImage(const StbImage&) = delete;
    StbImage(StbImage&& rhs) noexcept;

    ~StbImage();

    StbImage& operator=(const StbImage&) = delete;
    StbImage& operator=(StbImage&& rhs) noexcept;

    int width() const { return width_; }
    int height() const { return height_; }
    int channels() const { return channels_; }

    const unsigned char* pixels() const { return pixels_; }
    size_t size() const { return size_; }

private:
    int width_, height_, channels_;
    unsigned char* pixels_;
    size_t size_;
};
}  // namespace maseya::vkbase
