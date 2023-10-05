#include "SwapchainImage.hxx"

namespace maseya::vkbase {
SwapchainImage::SwapchainImage(VkSwapchainKHR swapchain, uint32_t image_index,
                               VkFormat format, const VkExtent2D& extent, VkImage image)
        : ImageBase(format, VK_IMAGE_TYPE_2D, extent, image),
          swapchain_(swapchain),
          image_index_(image_index) {}
}  // namespace maseya::vkbase