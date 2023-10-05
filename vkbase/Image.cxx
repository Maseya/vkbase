#include "Image.hxx"

// Local headers
#include "VulkanError.hxx"

namespace maseya::vkbase {
Image::Destroyer::Destroyer(VmaAllocator allocator) noexcept : allocator(allocator) {}

void Image::Destroyer::operator()(vma_image& image) const noexcept {
    destroy_image(allocator, image);
}

Image::Image(VkFormat format, const VkExtent2D& extent, VkImageType image_type,
             UniqueImage&& image)
        : ImageBase(format, image_type, extent, image->image),
          image_(std::move(image)) {}

Image::Image(VkDevice device, VmaAllocator allocator, VkFormat image_format,
             uint32_t extent)
        : Image(image_format, {extent, 1}, VK_IMAGE_TYPE_1D,
                UniqueImage(create_image(allocator, extent, image_format), allocator)) {
}

Image::Image(VkDevice device, VmaAllocator allocator, VkFormat image_format,
             const VkExtent2D& extent)
        : Image(image_format, extent, VK_IMAGE_TYPE_2D,
                UniqueImage(create_image(allocator, extent, image_format), allocator)) {
}
}  // namespace maseya::vkbase