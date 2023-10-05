#include "Fence.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
Fence::Destroyer::Destroyer(VkDevice device) noexcept : device(device) {}

void Fence::Destroyer::operator()(VkFence fence) const noexcept {
    vkDestroyFence(device, fence, nullptr);
}

Fence::Fence(VkDevice device, VkFenceCreateFlags flags)
        : fence_(create_fence(device, flags), device) {}
}  // namespace maseya::vkbase