#include "CommandPool.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
CommandPool::Destroyer::Destroyer(VkDevice device) noexcept : device(device) {}

void CommandPool::Destroyer::operator()(VkCommandPool command_pool) const noexcept {
    vkDestroyCommandPool(device, command_pool, nullptr);
}

CommandPool::CommandPool(VkDevice device, uint32_t queue_family_index)
        : command_pool_(VK_NULL_HANDLE, device) {
    command_pool_.reset(create_command_pool(device, queue_family_index));
}
}  // namespace maseya::vkbase