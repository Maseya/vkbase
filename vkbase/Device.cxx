#include "Device.hxx"

#include "vma_helper.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
void Device::DeviceDestroyer::operator()(VkDevice device) const noexcept {
    vkDestroyDevice(device, nullptr);
}

void Device::AllocatorDestroyer::operator()(VmaAllocator allocator) const noexcept {
    vmaDestroyAllocator(allocator);
}

Device::Device(VkInstance instance, VkPhysicalDevice physical_device,
               const std::unordered_set<uint32_t>& queue_family_indices,
               VkFormat default_image_format)
        : device_(create_device(physical_device, queue_family_indices)),
          allocator_(create_allocator(instance, physical_device, *device_)),
          default_image_format_(default_image_format) {}

Device::~Device() {
    if (device_) {
        wait_idle();
    }
}

void Device::wait_idle() const { assert_result(vkDeviceWaitIdle(*device_)); }
}  // namespace maseya::vkbase