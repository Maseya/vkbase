#pragma once

#include <vulkan/vulkan_core.h>

#include "RenderPass.hxx"

namespace maseya::vkbase {
class RenderPassFactory {
public:
    RenderPassFactory(VkDevice device, VkFormat default_image_format,
                      VkFormat default_depth_format);

    RenderPass create_flat_render_pass() const {
        return create_flat_render_pass(default_image_format_);
    }
    RenderPass create_flat_render_pass(VkImageLayout final_layout) const {
        return create_flat_render_pass(default_image_format_, final_layout);
    }
    RenderPass create_flat_render_pass(VkImageLayout initial_layout,
                                       VkImageLayout final_layout) const {
        return create_flat_render_pass(default_image_format_, initial_layout,
                                       final_layout);
    }

    RenderPass create_flat_render_pass(VkFormat image_format) const;
    RenderPass create_flat_render_pass(VkFormat image_format,
                                       VkImageLayout final_layout) const;
    RenderPass create_flat_render_pass(VkFormat image_format,
                                       VkImageLayout initial_layout,
                                       VkImageLayout final_layout) const;

    RenderPass create_depth_render_pass() const {
        return create_depth_render_pass(default_image_format_, default_depth_format_);
    }
    RenderPass create_depth_render_pass(VkImageLayout final_layout) const {
        return create_depth_render_pass(default_image_format_, default_depth_format_,
                                        final_layout);
    }
    RenderPass create_depth_render_pass(VkImageLayout initial_layout,
                                        VkImageLayout final_layout) const {
        return create_depth_render_pass(default_image_format_, default_depth_format_,
                                        initial_layout, final_layout);
    }

    RenderPass create_depth_render_pass(VkFormat image_format,
                                        VkFormat depth_format) const;
    RenderPass create_depth_render_pass(VkFormat image_format, VkFormat depth_format,
                                        VkImageLayout final_layout) const;
    RenderPass create_depth_render_pass(VkFormat image_format, VkFormat depth_format,
                                        VkImageLayout initial_layout,
                                        VkImageLayout final_layout) const;

private:
    VkDevice device_;
    VkFormat default_image_format_;
    VkFormat default_depth_format_;
};
}  // namespace maseya::vkbase
