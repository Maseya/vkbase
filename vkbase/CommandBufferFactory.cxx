#include "CommandBufferFactory.hxx"

namespace maseya::vkbase {
CommandBufferFactory::CommandBufferFactory(VkDevice device, VkCommandPool command_pool)
        : device_(device), command_pool_(command_pool) {}

CommandBuffer CommandBufferFactory::create_command_buffer() const {
    return CommandBuffer(device_, command_pool_);
}
}  // namespace maseya::vkbase