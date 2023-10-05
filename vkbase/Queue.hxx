#pragma once

#include <vulkan/vulkan_core.h>

namespace maseya::vkbase {
class Queue {
public:
    Queue(VkDevice device, uint32_t queue_family_index, uint32_t queue_index = 0);

    VkDevice device() const { return device_; }
    uint32_t queue_family_index() const { return queue_family_index_; }
    uint32_t queue_index() const { return queue_index_; }
    VkQueue operator*() const { return queue_; }

    void wait_idle() const;

private:
    VkDevice device_;
    uint32_t queue_family_index_;
    uint32_t queue_index_;
    VkQueue queue_;
};
}  // namespace maseya::vkbase