#include "FlatFramebuffer.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
FlatFramebuffer::Destroyer::Destroyer(VkDevice device) noexcept : device(device) {}

void FlatFramebuffer::Destroyer::operator()(VkFramebuffer framebuffer) const noexcept {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
}

FlatFramebuffer::FlatFramebuffer(VkDevice device, VkRenderPass flat_render_pass,
                                 const VkExtent2D& extent, VkFormat format,
                                 VkImage image)
        : device_(device),
          extent_(extent),
          image_view_(device, image, format),
          framebuffer_(
                  create_framebuffer(device, *image_view_, flat_render_pass, extent),
                  device) {}
}  // namespace maseya::vkbase