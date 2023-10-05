#include "Semaphore.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
Semaphore::Destroyer::Destroyer(VkDevice device) noexcept : device(device) {}

void Semaphore::Destroyer::operator()(VkSemaphore semaphore) const noexcept {
    vkDestroySemaphore(device, semaphore, nullptr);
}

Semaphore::Semaphore(VkDevice device) : semaphore_(create_semaphore(device), device) {}
}  // namespace maseya::vkbase