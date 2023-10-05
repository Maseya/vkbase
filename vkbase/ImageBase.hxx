#pragma once

#include <vulkan/vulkan_core.h>

#include <cstddef>

namespace maseya::vkbase {
class ImageBase {
protected:
    constexpr ImageBase(std::nullptr_t) noexcept
            : format_{}, image_type_{}, extent_{}, image_(VK_NULL_HANDLE) {}

    ImageBase(VkFormat format, VkImageType image_type, const VkExtent2D& extent,
              VkImage image);

public:
    ImageBase(const ImageBase&) = delete;
    ImageBase(ImageBase&&) noexcept = default;

    ImageBase& operator=(const ImageBase&) = delete;
    ImageBase& operator=(ImageBase&&) noexcept = default;

    VkFormat format() const noexcept { return format_; }
    VkImageType image_type() const noexcept { return image_type_; }
    const VkExtent2D& extent() const noexcept { return extent_; }
    VkImage operator*() const noexcept { return image_; }

    constexpr explicit operator bool() const noexcept {
        return static_cast<bool>(image_);
    }

private:
    VkFormat format_;
    VkImageType image_type_;
    VkExtent2D extent_;
    VkImage image_;
};
}  // namespace maseya::vkbase
