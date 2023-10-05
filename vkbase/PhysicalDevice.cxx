#include "PhysicalDevice.hxx"

#include <string>

#include "VulkanError.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
PhysicalDevice::PhysicalDevice(VkInstance instance, VkPhysicalDevice physical_device)
        : instance_(instance), physical_device_(physical_device) {}

VkPhysicalDeviceProperties PhysicalDevice::properties() const {
    return get_physical_device_properties(physical_device_);
}

std::vector<VkExtensionProperties> PhysicalDevice::extension_properties() const {
    return get_device_extension_properties(physical_device_);
}

std::vector<VkQueueFamilyProperties> PhysicalDevice::queue_family_properties() const {
    return get_queue_family_properties(physical_device_);
}
}  // namespace maseya::vkbase