#include "FlatFramebufferFactory.hxx"

namespace maseya::vkbase {
FlatFramebufferFactory::FlatFramebufferFactory(VkDevice device,
                                               VkRenderPass flat_render_pass)
        : device_(device), flat_render_pass_(flat_render_pass) {}

FlatFramebuffer FlatFramebufferFactory::create_framebuffer(const VkExtent2D& extent,
                                                           VkFormat format,
                                                           VkImage image) const {
    return FlatFramebuffer(device_, flat_render_pass_, extent, format, image);
}

FlatFramebuffer FlatFramebufferFactory::create_framebuffer(
        const ImageBase& image) const {
    return create_framebuffer(image.extent(), image.format(), *image);
}

std::vector<FlatFramebuffer> FlatFramebufferFactory::create_swapchain_framebuffers(
        const VkExtent2D& extent, VkFormat format, const VkImage* swapchain_images,
        uint32_t image_count) const {
    std::vector<FlatFramebuffer> result;
    for (uint32_t i = 0; i < image_count; i++) {
        result.push_back(create_framebuffer(extent, format, swapchain_images[i]));
    }

    return result;
}

std::vector<FlatFramebuffer> FlatFramebufferFactory::create_swapchain_framebuffers(
        const Swapchain& swapchain) const {
    std::vector<FlatFramebuffer> result;
    for (const SwapchainImage& image : swapchain.images()) {
        result.push_back(create_framebuffer(image));
    }

    return result;
}
}  // namespace maseya::vkbase