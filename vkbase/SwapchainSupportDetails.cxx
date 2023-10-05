#include "SwapchainSupportDetails.hxx"

#include <algorithm>
#include <cstdint>
#include <unordered_set>

#include "PresentationQueueFamilyIndices.hxx"
#include "VulkanError.hxx"
#include "vulkan_helper.hxx"
#include "vulkan_surface_helper.hxx"

namespace maseya::vkbase {
SwapchainSupportDetails::SwapchainSupportDetails(VkPhysicalDevice physical_device,
                                                 VkSurfaceKHR surface,
                                                 const VkExtent2D& desired_extent)
        : physical_device_(physical_device),
          surface_(surface),
          surface_format_(pick_surface_format(physical_device, surface)),
          present_mode_(pick_present_mode(physical_device, surface)),
          extent_(get_extent(physical_device, surface, desired_extent)) {}

SwapchainSupportDetails::SwapchainSupportDetails(VkPhysicalDevice physical_device,
                                                 VkSurfaceKHR surface,
                                                 VkSurfaceFormatKHR surface_format,
                                                 VkPresentModeKHR present_mode,
                                                 const VkExtent2D& extent)
        : physical_device_(physical_device),
          surface_(surface),
          surface_format_(surface_format),
          present_mode_(present_mode),
          extent_(extent) {}

bool SwapchainSupportDetails::is_supported() const {
    return !is_minimized(extent_) && surface_format_.has_value() &&
           present_mode_.has_value();
}

constexpr static bool no_max_image_count(const VkSurfaceCapabilitiesKHR& capabilities) {
    return capabilities.maxImageCount == 0;
}

VkSwapchainKHR SwapchainSupportDetails::create_swapchain(
        VkDevice device, VkSwapchainKHR old_swapchain) const {
    VkSurfaceCapabilitiesKHR capabilities =
            get_surface_capabilities(physical_device_, surface_);

    // Try to request at least one image more than minimum to avoid having to wait for
    // the driver to complete internal operations, which would make us unable to acquire
    // another image to render to.
    uint32_t image_count = capabilities.minImageCount + 1;

    // Make sure we do not exceed the maximum number of images. If the max is set to 0,
    // that is a special value saying there is no max.
    if (!no_max_image_count(capabilities) && image_count > capabilities.maxImageCount) {
        image_count = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface_;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format().format;
    create_info.imageColorSpace = surface_format().colorSpace;
    create_info.imageExtent = extent_;
    create_info.presentMode = present_mode();
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    PresentationQueueFamilyIndices indices(physical_device_, surface_);
    std::unordered_set<uint32_t> queue_family_indices_set(
            {indices.graphics_family(), indices.presentation_family()});
    std::vector<uint32_t> queue_family_indices(queue_family_indices_set.begin(),
                                               queue_family_indices_set.end());

    create_info.queueFamilyIndexCount =
            static_cast<uint32_t>(queue_family_indices.size());
    create_info.pQueueFamilyIndices = queue_family_indices.data();
    if (indices.graphics_family() != indices.presentation_family()) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.clipped = true;

    // It's possible that our swap chain can become invalidated or unoptimized while the
    // program is running (e.g. if the window is resized). In such a case, the swap
    // chain needs to be entirely recreated from scratch and a reference to the old one
    // must be specified in this field.
    create_info.oldSwapchain = old_swapchain;

    VkSwapchainKHR result;
    assert_result(vkCreateSwapchainKHR(device, &create_info, nullptr, &result));

    return result;
}
}  // namespace maseya::vkbase