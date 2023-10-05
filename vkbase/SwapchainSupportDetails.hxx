#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>
#include <vector>

namespace maseya::vkbase {
class SwapchainSupportDetails {
public:
    SwapchainSupportDetails(VkPhysicalDevice physical_device, VkSurfaceKHR surface,
                            const VkExtent2D& desired_extent = {});

    SwapchainSupportDetails(VkPhysicalDevice physical_device, VkSurfaceKHR surface,
                            VkSurfaceFormatKHR surface_format,
                            VkPresentModeKHR present_mode, const VkExtent2D& extent);

    bool is_supported() const;

    const VkSurfaceFormatKHR& surface_format() const { return surface_format_.value(); }
    VkFormat format() const { return surface_format().format; }
    VkPresentModeKHR present_mode() const { return present_mode_.value(); }
    const VkExtent2D& extent() const { return extent_; }
    VkSwapchainKHR create_swapchain(VkDevice device,
                                    VkSwapchainKHR old_swapchain = {}) const;

private:
    VkPhysicalDevice physical_device_;
    VkSurfaceKHR surface_;
    std::optional<VkSurfaceFormatKHR> surface_format_;
    std::optional<VkPresentModeKHR> present_mode_;
    VkExtent2D extent_;
};
}  // namespace maseya::vkbase