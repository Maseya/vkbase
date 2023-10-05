#include "SwapchainFactory.hxx"

namespace maseya::vkbase {
SwapchainFactory::SwapchainFactory(VkPhysicalDevice physical_device, VkDevice device)
        : physical_device_(physical_device), device_(device) {}

SwapchainSupportDetails SwapchainFactory::get_support_details(
        VkSurfaceKHR surface) const {
    return SwapchainSupportDetails(physical_device_, surface);
}

Swapchain SwapchainFactory::create_swapchain(VkSurfaceKHR surface) const {
    SwapchainSupportDetails support_details(get_support_details(surface));
    if (!support_details.is_supported()) {
        return Swapchain(nullptr);
    }

    return Swapchain(device_, support_details);
}

ManagedSwapchain SwapchainFactory::create_managed_swapchain(
        VkCommandPool command_pool, VkSurfaceKHR surface) const {
    return ManagedSwapchain(physical_device_, device_, command_pool, surface);
}
}  // namespace maseya::vkbase