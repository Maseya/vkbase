#include "StbImage.hxx"

#include "VulkanError.hxx"
#include "stb_image.h"
#include "stb_image_write.h"

namespace maseya::vkbase {
StbImage::StbImage(int width, int height, int channels, unsigned char* pixels)
        : width_(width),
          height_(height),
          channels_(channels),
          pixels_(pixels),
          size_(static_cast<size_t>(width) * height * 4) {}

StbImage::StbImage(const char* path) : StbImage() {
    pixels_ = stbi_load(path, &width_, &height_, &channels_, STBI_rgb_alpha);
    if (!pixels_) {
        throw VkBaseError("failed to load texture image!");
    }

    size_ = static_cast<size_t>(width_) * height_ * 4;
}

StbImage::StbImage(StbImage&& rhs) noexcept
        : width_(rhs.width_),
          height_(rhs.height_),
          channels_(rhs.channels_),
          pixels_(std::exchange(rhs.pixels_, nullptr)),
          size_(rhs.size_) {}

StbImage ::~StbImage() { stbi_image_free(pixels_); }

StbImage& StbImage::operator=(StbImage&& rhs) noexcept {
    std::swap(width_, rhs.width_);
    std::swap(height_, rhs.height_);
    std::swap(channels_, rhs.channels_);
    std::swap(pixels_, rhs.pixels_);
    std::swap(size_, rhs.size_);
    return *this;
}
}  // namespace maseya::vkbase