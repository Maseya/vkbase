#include "Swapchain.hxx"

#include "vulkan_surface_helper.hxx"

namespace maseya::vkbase {
Swapchain::Destroyer::Destroyer(VkDevice device) noexcept : device(device) {}

void Swapchain::Destroyer::operator()(VkSwapchainKHR swapchain) const noexcept {
    vkDestroySwapchainKHR(device, swapchain, nullptr);
}

Swapchain::Swapchain(VkDevice device, const SwapchainSupportDetails& support_details,
                     VkSwapchainKHR old_swapchain)
        : swapchain_(VK_NULL_HANDLE, device), images_() {
    swapchain_.reset(support_details.create_swapchain(device, old_swapchain));
    std::vector<VkImage> images = get_swapchain_images(device, *swapchain_);
    for (uint32_t i = 0; i < images.size(); i++) {
        images_.emplace_back(*swapchain_, i, support_details.format(),
                             support_details.extent(), images[i]);
    }
}
}  // namespace maseya::vkbase