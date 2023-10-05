#pragma once

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include "Image.hxx"

namespace maseya::vkbase {
class ImageFactory {
public:
    ImageFactory(VkDevice device, VmaAllocator allocator,
                 VkFormat default_image_format);

    Image create_image(VkFormat format, uint32_t width);
    Image create_image(const uint32_t width) {
        return create_image(default_image_format_, width);
    }

    Image create_image(VkFormat format, const VkExtent2D& extent);
    Image create_image(const VkExtent2D& extent) {
        return create_image(default_image_format_, extent);
    }

private:
    VkDevice device_;
    VmaAllocator allocator;
    VkFormat default_image_format_;
};
}  // namespace maseya::vkbase
