#include "Queue.hxx"

#include "VulkanError.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
Queue::Queue(VkDevice device, uint32_t queue_family_index, uint32_t queue_index)
        : device_(device),
          queue_family_index_(queue_family_index),
          queue_index_(queue_index),
          queue_(get_queue(device, queue_family_index, queue_index)) {}

void Queue::wait_idle() const { assert_result(vkQueueWaitIdle(queue_)); }
}  // namespace maseya::vkbase