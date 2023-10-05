#pragma once

#include <vulkan/vulkan.h>

#include "Device.hxx"
#include "ImageBase.hxx"
#include "UniqueObject.hxx"
#include "vma_helper.hxx"

namespace maseya::vkbase {
class Image : public ImageBase {
    struct Destroyer {
        constexpr Destroyer() noexcept : allocator(nullptr) {}

        Destroyer(VmaAllocator allocator) noexcept;

        void operator()(vma_image& image) const noexcept;

        VmaAllocator allocator;
    };

    using UniqueImage = UniqueObject<vma_image, Destroyer>;

public:
    constexpr Image(std::nullptr_t) noexcept : ImageBase(nullptr), image_(nullptr) {}

private:
    Image(VkFormat format, const VkExtent2D& extent, VkImageType image_type,
          UniqueImage&& image);

public:
    Image(VkDevice device, VmaAllocator allocator, VkFormat image_format,
          uint32_t extent);
    Image(VkDevice device, VmaAllocator allocator, VkFormat image_format,
          const VkExtent2D& extent);

    Image(const Image&) = delete;
    Image(Image&&) noexcept = default;

    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&) noexcept = default;

private:
    UniqueImage image_;
};
}  // namespace maseya::vkbase
