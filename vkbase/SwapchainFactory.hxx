#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>

#include "ManagedSwapchain.hxx"
#include "Surface.hxx"
#include "Swapchain.hxx"

namespace maseya::vkbase {
class SwapchainFactory {
public:
    SwapchainFactory(VkPhysicalDevice physical_device, VkDevice device);

    SwapchainSupportDetails get_support_details(VkSurfaceKHR surface) const;
    Swapchain create_swapchain(VkSurfaceKHR surface) const;

    ManagedSwapchain create_managed_swapchain(VkCommandPool command_pool,
                                              VkSurfaceKHR surface) const;

private:
    VkPhysicalDevice physical_device_;
    VkDevice device_;
};
}  // namespace maseya::vkbase
