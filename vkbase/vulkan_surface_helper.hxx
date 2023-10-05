#pragma once

#include <optional>
#include <vector>

#include "vulkan_helper.hxx"

constexpr bool operator==(const VkSurfaceFormatKHR& lhs,
                          const VkSurfaceFormatKHR& rhs) {
    return lhs.colorSpace == rhs.colorSpace && lhs.format == rhs.format;
}
constexpr bool operator!=(const VkSurfaceFormatKHR& lhs,
                          const VkSurfaceFormatKHR& rhs) {
    return !(lhs == rhs);
}

namespace maseya::vkbase {
// TODO(nrg): Some Linux platforms will only support the R8G8B8 format. Likewise, some
// macOS systems only support extended or HDR color systems. Just something to watch out
// for.
#ifndef VK_USE_PLATFORM_ANDROID_KHR
constexpr VkFormat preferred_surface_format = VK_FORMAT_B8G8R8A8_UNORM;
#else
constexpr VkFormat preferred_surface_format = VK_FORMAT_R8G8B8A8_UNORM;
#endif

VkBool32 queue_supports_presentation(VkPhysicalDevice physical_device,
                                     uint32_t queue_family_index, VkSurfaceKHR surface);

std::optional<uint32_t> get_presentation_queue_family_index(
        VkPhysicalDevice physical_device, VkSurfaceKHR surface);

VkSurfaceCapabilitiesKHR get_surface_capabilities(VkPhysicalDevice physical_device,
                                                  VkSurfaceKHR surface);

std::vector<VkSurfaceFormatKHR> get_surface_formats(VkPhysicalDevice physical_device,
                                                    VkSurfaceKHR surface);

std::vector<VkPresentModeKHR> get_present_modes(VkPhysicalDevice physical_device,
                                                VkSurfaceKHR surface);

std::optional<VkSurfaceFormatKHR> pick_surface_format(
        VkPhysicalDevice physical_device, VkSurfaceKHR surface,
        VkSurfaceFormatKHR preferred_format = {preferred_surface_format,
                                               VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        VkBool32 force_preferred_format = VK_FALSE);

std::optional<VkPresentModeKHR> pick_present_mode(
        VkPhysicalDevice physical_device, VkSurfaceKHR surface,
        VkPresentModeKHR preferred_mode = preferred_present_mode,
        VkBool32 force_preferred_mode = VK_FALSE);

VkExtent2D get_extent(VkPhysicalDevice physical_device, VkSurfaceKHR surface,
                      const VkExtent2D& desired_extent = {});

template <class Comparer>
VkPhysicalDevice pick_physical_device(
        const std::vector<VkPhysicalDevice>& physical_devices,
        const Comparer& comparer) {
    if (physical_devices.empty()) {
        throw VkBaseError("Failed to find any GPU with Vulkan support.");
    }

    // Select the best rated physical device.
    VkPhysicalDevice result = *std::max_element(physical_devices.begin(),
                                                physical_devices.end(), comparer);

    // Check that the returned device was suitable
    if (!comparer(nullptr, result)) {
        throw VkBaseError("Failed to find a GPU meeting minimum requirements.");
    }

    return result;
}

template <class Comparer>
VkPhysicalDevice pick_physical_device(VkInstance instance, const Comparer& comparer) {
    return pick_physical_device(get_physical_devices(instance), comparer);
}

template <class Comparer>
VkPhysicalDevice pick_physical_device(VkInstance instance, VkSurfaceKHR surface) {
    return pick_physical_device(instance, Comparer(instance, surface));
}

std::vector<VkImage> get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain);
}  // namespace maseya::vkbase
