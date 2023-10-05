#pragma once

#include <vector>

#include "Device.hxx"
#include "FlatFramebuffer.hxx"
#include "ImageBase.hxx"
#include "Swapchain.hxx"

namespace maseya::vkbase {
class FlatFramebufferFactory {
public:
    FlatFramebufferFactory(VkDevice device, VkRenderPass flat_render_pass);

    FlatFramebuffer create_framebuffer(const VkExtent2D& extent, VkFormat format,
                                       VkImage image) const;

    FlatFramebuffer create_framebuffer(const ImageBase& image) const;

    std::vector<FlatFramebuffer> create_swapchain_framebuffers(
            const VkExtent2D& extent, VkFormat format, const VkImage* swapchain_images,
            uint32_t image_count) const;

    std::vector<FlatFramebuffer> create_swapchain_framebuffers(
            const Swapchain& swapchain) const;

private:
    VkDevice device_;
    VkRenderPass flat_render_pass_;
};
}  // namespace maseya::vkbase
