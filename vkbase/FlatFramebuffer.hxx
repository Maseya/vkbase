#pragma once

#include <vulkan/vulkan_core.h>

#include "ImageBase.hxx"
#include "ImageView.hxx"
#include "UniqueObject.hxx"

namespace maseya::vkbase {
class FlatFramebuffer {
    struct Destroyer {
        constexpr Destroyer() noexcept : device(nullptr) {}

        Destroyer(VkDevice device) noexcept;

        void operator()(VkFramebuffer framebuffer) const noexcept;

        VkDevice device;
    };

public:
    constexpr FlatFramebuffer(std::nullptr_t) noexcept
            : device_(nullptr),
              extent_{},
              image_view_(nullptr),
              framebuffer_(nullptr) {}

    FlatFramebuffer(VkDevice device, VkRenderPass flat_render_pass,
                    const VkExtent2D& extent, VkFormat format, VkImage image);

    FlatFramebuffer(const FlatFramebuffer&) = delete;
    FlatFramebuffer(FlatFramebuffer&&) noexcept = default;

    FlatFramebuffer& operator=(const FlatFramebuffer&) = delete;
    FlatFramebuffer& operator=(FlatFramebuffer&&) noexcept = default;

    VkDevice device() const { return device_; }
    const VkExtent2D& extent() const { return extent_; }
    VkImageView image_view() const { return *image_view_; }
    VkFramebuffer operator*() const { return *framebuffer_; }

private:
    VkDevice device_;
    VkExtent2D extent_;
    ImageView image_view_;
    UniqueObject<VkFramebuffer, Destroyer> framebuffer_;
};
}  // namespace maseya::vkbase
