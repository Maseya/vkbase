#pragma once

#include "Queue.hxx"
#include "SwapchainImage.hxx"

namespace maseya::vkbase {
class PresentationQueue : public Queue {
public:
    PresentationQueue(VkDevice device, uint32_t queue_family_index,
                      uint32_t queue_index = 0);

    bool present(const SwapchainImage& image,
                 VkSemaphore semaphore = VK_NULL_HANDLE) const;
};
}  // namespace maseya::vkbase
