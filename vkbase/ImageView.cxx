#include "ImageView.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
ImageView::Destroyer::Destroyer(VkDevice device) noexcept : device(device) {}

void ImageView::Destroyer::operator()(VkImageView image_view) const noexcept {
    vkDestroyImageView(device, image_view, nullptr);
}

ImageView::ImageView(VkDevice device, VkImage image, VkFormat format,
                     VkImageAspectFlags aspect_mask, VkImageViewType image_view_type)
        : image_view_(create_image_view(device, image, format, aspect_mask,
                                        image_view_type),
                      device) {}

}  // namespace maseya::vkbase