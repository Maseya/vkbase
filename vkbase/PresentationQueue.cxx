#include "PresentationQueue.hxx"

#include <vector>

#include "VulkanError.hxx"

namespace maseya::vkbase {
PresentationQueue::PresentationQueue(VkDevice device, uint32_t queue_family_index,
                                     uint32_t queue_index)
        : Queue(device, queue_family_index, queue_index) {}

bool PresentationQueue::present(const SwapchainImage& image,
                                VkSemaphore wait_semaphore) const {
    VkSwapchainKHR swapchain = image.swapchain();
    uint32_t image_index = image.image_index();

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    if (wait_semaphore) {
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &wait_semaphore;
    }
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain;
    present_info.pImageIndices = &image_index;

    VkResult result = vkQueuePresentKHR(operator*(), &present_info);
    switch (result) {
        case VK_SUBOPTIMAL_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR:
            return false;
        default:
            assert_result(result);
            break;
    }

    return true;
}
}  // namespace maseya::vkbase