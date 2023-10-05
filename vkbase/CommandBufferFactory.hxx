#pragma once

#include <vulkan/vulkan_core.h>

#include "CommandBuffer.hxx"

namespace maseya::vkbase {
class CommandBufferFactory {
public:
    CommandBufferFactory(VkDevice device, VkCommandPool command_pool);

    CommandBuffer create_command_buffer() const;

private:
    VkDevice device_;
    VkCommandPool command_pool_;
};
}  // namespace maseya::vkbase
