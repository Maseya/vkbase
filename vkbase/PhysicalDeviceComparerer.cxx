#include "PhysicalDeviceComparerer.hxx"

#include <algorithm>

#include "PresentationQueueFamilyIndices.hxx"
#include "SwapchainSupportDetails.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
PhysicaDeviceComparer::PhysicaDeviceComparer(VkInstance instance, VkSurfaceKHR surface)
        : instance_(instance), surface_(surface) {}

bool PhysicaDeviceComparer::operator()(VkPhysicalDevice lhs,
                                       VkPhysicalDevice rhs) const {
    return score_physical_device(lhs) < score_physical_device(rhs);
}

std::uint32_t PhysicaDeviceComparer::score_physical_device(
        VkPhysicalDevice physical_device) const {
    if (!physical_device) {
        return 0;
    }

    if (!get_unsupported_device_extensions(physical_device).empty()) {
        return 0;
    }

    PresentationQueueFamilyIndices indices(physical_device, surface_);
    if (!indices.is_complete()) {
        return 0;
    }

    SwapchainSupportDetails details(physical_device, surface_);
    if (!details.is_supported()) {
        return 0;
    }

    if (details.format() == VK_FORMAT_UNDEFINED) {
        return 0;
    }

    // Any physical devices so far support our required queues and device
    // extensions. We start with a positive score to mark the device as
    // "suitable". Higher scores make a device more suitable.
    std::uint32_t score = 1;

    // Use physical device properties to determine suitability.
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device, &properties);

    // A discrete GPU will offer much better performance compared to an integrated
    // card. In any case, this is the most important factor.
    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // This surface format gives the best visual representation of Super NES
    // graphics.
    if (details.format() == VK_FORMAT_B8G8R8A8_UNORM) {
        score += 10;
    }

    // Do something with this one day.
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(physical_device, details.format(),
                                        &format_properties);

    if (details.present_mode() == VK_PRESENT_MODE_MAILBOX_KHR) {
        score += 10;
    }

    return score;
}
}  // namespace maseya::vkbase