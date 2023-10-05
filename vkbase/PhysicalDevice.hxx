#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

#include "QueueFamilyIndices.hxx"

namespace maseya::vkbase {
class PhysicalDevice {
public:
    PhysicalDevice(VkInstance instance, VkPhysicalDevice physical_device);

    VkInstance instance() const { return instance_; }
    VkPhysicalDevice operator*() const { return physical_device_; }

    std::string name() const { return properties().deviceName; }

    VkPhysicalDeviceProperties properties() const;
    std::vector<VkExtensionProperties> extension_properties() const;
    std::vector<VkQueueFamilyProperties> queue_family_properties() const;

private:
    VkInstance instance_;
    VkPhysicalDevice physical_device_;
};
}  // namespace maseya::vkbase
