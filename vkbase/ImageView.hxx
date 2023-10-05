#pragma once

#include <vulkan/vulkan_core.h>

#include "UniqueObject.hxx"

namespace maseya::vkbase {
class ImageView {
    struct Destroyer {
        constexpr Destroyer() noexcept : device(nullptr) {}

        Destroyer(VkDevice device) noexcept;

        void operator()(VkImageView image_view) const noexcept;

        VkDevice device;
    };

public:
    constexpr ImageView(std::nullptr_t) noexcept : image_view_(nullptr) {}

    ImageView(VkDevice device, VkImage image, VkFormat format,
              VkImageAspectFlags aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT,
              VkImageViewType image_view_type = VK_IMAGE_VIEW_TYPE_2D);

    ImageView(const ImageView&) = delete;
    ImageView(ImageView&&) noexcept = default;

    ImageView& operator=(const ImageView&) = delete;
    ImageView& operator=(ImageView&&) noexcept = default;

    VkImageView operator*() const noexcept { return *image_view_; }

private:
    UniqueObject<VkImageView, Destroyer> image_view_;
};
}  // namespace maseya::vkbase
