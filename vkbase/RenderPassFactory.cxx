#include "RenderPassFactory.hxx"

namespace maseya::vkbase {
RenderPassFactory::RenderPassFactory(VkDevice device, VkFormat default_image_format,
                                     VkFormat default_depth_format)
        : device_(device),
          default_image_format_(default_image_format),
          default_depth_format_(default_depth_format) {}

RenderPass RenderPassFactory::create_flat_render_pass(VkFormat image_format) const {
    return RenderPass(device_, image_format);
}
RenderPass RenderPassFactory::create_flat_render_pass(
        VkFormat image_format, VkImageLayout final_layout) const {
    return RenderPass(device_, image_format, final_layout);
}
RenderPass RenderPassFactory::create_flat_render_pass(
        VkFormat image_format, VkImageLayout initial_layout,
        VkImageLayout final_layout) const {
    return RenderPass(device_, image_format, initial_layout, final_layout);
}

RenderPass RenderPassFactory::create_depth_render_pass(VkFormat image_format,
                                                       VkFormat depth_format) const {
    return RenderPass(device_, image_format, depth_format);
}
RenderPass RenderPassFactory::create_depth_render_pass(
        VkFormat image_format, VkFormat depth_format,
        VkImageLayout final_layout) const {
    return RenderPass(device_, image_format, depth_format, final_layout);
}
RenderPass RenderPassFactory::create_depth_render_pass(
        VkFormat image_format, VkFormat depth_format, VkImageLayout initial_layout,
        VkImageLayout final_layout) const {
    return RenderPass(device_, image_format, depth_format, initial_layout,
                      final_layout);
}
}  // namespace maseya::vkbase