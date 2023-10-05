#pragma once

#include <vulkan/vulkan_core.h>

#include "ImageBase.hxx"

namespace maseya::vkbase {
class SwapchainImage : public ImageBase {
public:
    constexpr SwapchainImage(std::nullptr_t)
            : ImageBase(nullptr), swapchain_(VK_NULL_HANDLE), image_index_{} {}

    SwapchainImage(VkSwapchainKHR swapchain, uint32_t image_index, VkFormat format,
                   const VkExtent2D& extent, VkImage image);

    VkSwapchainKHR swapchain() const { return swapchain_; }
    uint32_t image_index() const { return image_index_; }

private:
    VkSwapchainKHR swapchain_;
    uint32_t image_index_;
};
}  // namespace maseya::vkbase
