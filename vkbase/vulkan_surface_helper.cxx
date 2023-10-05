#include "vulkan_surface_helper.hxx"

#include "PhysicalDeviceComparerer.hxx"
#include "VulkanError.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
VkBool32 queue_supports_presentation(VkPhysicalDevice physical_device,
                                     uint32_t queue_family_index,
                                     VkSurfaceKHR surface) {
    VkBool32 result;
    assert_result(vkGetPhysicalDeviceSurfaceSupportKHR(
            physical_device, queue_family_index, surface, &result));

    return result;
}

std::optional<uint32_t> get_presentation_queue_family_index(
        VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    std::vector<VkQueueFamilyProperties> families =
            get_queue_family_properties(physical_device);
    for (uint32_t i = 0; i < families.size(); i++) {
        if (queue_supports_presentation(physical_device, i, surface)) {
            return i;
        }
    }

    return std::nullopt;
}

VkSurfaceCapabilitiesKHR get_surface_capabilities(VkPhysicalDevice physical_device,
                                                  VkSurfaceKHR surface) {
    VkSurfaceCapabilitiesKHR result;
    assert_result(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface,
                                                            &result));

    return result;
}

std::vector<VkSurfaceFormatKHR> get_surface_formats(VkPhysicalDevice physical_device,
                                                    VkSurfaceKHR surface) {
    uint32_t count;
    assert_result(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count,
                                                       nullptr));

    std::vector<VkSurfaceFormatKHR> result(count);
    assert_result(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count,
                                                       result.data()));

    return result;
}

std::vector<VkPresentModeKHR> get_present_modes(VkPhysicalDevice physical_device,
                                                VkSurfaceKHR surface) {
    uint32_t count;
    assert_result(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface,
                                                            &count, nullptr));

    std::vector<VkPresentModeKHR> result(count);
    assert_result(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface,
                                                            &count, result.data()));

    return result;
}


std::optional<VkSurfaceFormatKHR> pick_surface_format(
        VkPhysicalDevice physical_device, VkSurfaceKHR surface,
        VkSurfaceFormatKHR preferred_format, VkBool32 force_preferred_format) {
    std::vector<VkSurfaceFormatKHR> formats =
            get_surface_formats(physical_device, surface);
    if (formats.empty()) {
        return std::nullopt;
    }

    auto result = std::find(formats.begin(), formats.end(), preferred_format);
    if (result == formats.end()) {
        if (force_preferred_format) {
            return std::nullopt;
        }

        // If we don't have our desired format, then any format will do and we can just
        // default to the first available format.
        return formats.front();
    }

    return *result;
}

std::optional<VkPresentModeKHR> pick_present_mode(VkPhysicalDevice physical_device,
                                                  VkSurfaceKHR surface,
                                                  VkPresentModeKHR preferred_mode,
                                                  VkBool32 force_preferred_mode) {
    std::vector<VkPresentModeKHR> modes = get_present_modes(physical_device, surface);

    auto result = std::find(modes.begin(), modes.end(), preferred_mode);
    if (result != modes.end()) {
        if (force_preferred_mode) {
            return std::nullopt;
        }

        // If desired mode not found, then FIFO is always guaranteed to exist, so we
        // fall back to that.
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    return *result;
}

constexpr static bool any_size_supported(const VkSurfaceCapabilitiesKHR& capabilities) {
    return capabilities.currentExtent.width == UINT32_MAX;
}

VkExtent2D get_extent(VkPhysicalDevice physical_device, VkSurfaceKHR surface,
                      const VkExtent2D& desired_extent) {
    VkSurfaceCapabilitiesKHR capabilities =
            get_surface_capabilities(physical_device, surface);

    // If the current extent width is set to "any size", and we set an actual desired
    // extent, then we return the actual extent, clamped between the minimum and maximum
    // actual extents.
    if (any_size_supported(capabilities) && desired_extent != VkExtent2D{}) {
        return clamp(desired_extent, capabilities.minImageExtent,
                     capabilities.maxImageExtent);
    }

    return capabilities.currentExtent;
}

std::vector<VkImage> get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain) {
    uint32_t count;
    assert_result(vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr));

    std::vector<VkImage> result(count);
    assert_result(vkGetSwapchainImagesKHR(device, swapchain, &count, result.data()));

    return result;
}
}  // namespace maseya::vkbase