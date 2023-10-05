#pragma once

#include "UniqueObject.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
class RenderPass {
    struct Destroyer {
        constexpr Destroyer() noexcept : device(nullptr) {}

        Destroyer(VkDevice device) noexcept;

        void operator()(VkRenderPass render_pass) const noexcept;

        VkDevice device;
    };

public:
    constexpr RenderPass(std::nullptr_t) noexcept : render_pass_(nullptr) {}

    RenderPass(VkDevice device, VkFormat image_format, VkImageLayout final_layout);
    RenderPass(VkDevice device, VkFormat image_format);
    RenderPass(VkDevice device, VkFormat image_format, VkImageLayout initial_layout,
               VkImageLayout final_layout);

    RenderPass(VkDevice device, VkFormat image_format, VkFormat depth_format);
    RenderPass(VkDevice device, VkFormat image_format, VkFormat depth_format,
               VkImageLayout final_layout);
    RenderPass(VkDevice device, VkFormat image_format, VkFormat depth_format,
               VkImageLayout initial_layout, VkImageLayout final_layout);

    VkRenderPass operator*() const noexcept { return *render_pass_; }

    explicit operator bool() const noexcept { return static_cast<bool>(render_pass_); }

private:
    UniqueObject<VkRenderPass, Destroyer> render_pass_;
};
}  // namespace maseya::vkbase
