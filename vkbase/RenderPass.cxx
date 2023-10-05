#include "RenderPass.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
RenderPass::Destroyer::Destroyer(VkDevice device) noexcept : device(device) {}

void RenderPass::Destroyer::operator()(VkRenderPass render_pass) const noexcept {
    vkDestroyRenderPass(device, render_pass, nullptr);
}

RenderPass::RenderPass(VkDevice device, VkFormat image_format)
        : render_pass_(create_flat_render_pass(device, image_format), device) {}
RenderPass::RenderPass(VkDevice device, VkFormat image_format,
                       VkImageLayout final_layout)
        : render_pass_(create_flat_render_pass(device, image_format, final_layout),
                       device) {}
RenderPass::RenderPass(VkDevice device, VkFormat image_format,
                       VkImageLayout initial_layout, VkImageLayout final_layout)
        : render_pass_(create_flat_render_pass(device, image_format, initial_layout,
                                               final_layout),
                       device) {}

RenderPass::RenderPass(VkDevice device, VkFormat image_format, VkFormat depth_format)
        : render_pass_(create_depth_render_pass(device, image_format, depth_format),
                       device) {}
RenderPass::RenderPass(VkDevice device, VkFormat image_format, VkFormat depth_format,
                       VkImageLayout final_layout)
        : render_pass_(create_depth_render_pass(device, image_format, depth_format,
                                                final_layout)) {}
RenderPass::RenderPass(VkDevice device, VkFormat image_format, VkFormat depth_format,
                       VkImageLayout initial_layout, VkImageLayout final_layout)
        : render_pass_(create_depth_render_pass(device, image_format, depth_format,
                                                initial_layout, final_layout),
                       device) {}
}  // namespace maseya::vkbase