#include "ImageFactory.hxx"

#include "vma_helper.hxx"

namespace maseya::vkbase {
ImageFactory::ImageFactory(VkDevice device, VmaAllocator allocator,
                           VkFormat default_image_format)
        : device_(device),
          allocator(allocator),
          default_image_format_(default_image_format) {}

Image ImageFactory::create_image(VkFormat format, uint32_t width) {
    return Image(device_, allocator, format, width);
}

Image ImageFactory::create_image(VkFormat format, const VkExtent2D& extent) {
    return Image(device_, allocator, format, extent);
}
}  // namespace maseya::vkbase