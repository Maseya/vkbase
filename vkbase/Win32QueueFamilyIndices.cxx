#include "Win32QueueFamilyIndices.hxx"

namespace maseya::vkbase::win32 {
Win32QueueFamilyIndices::Win32QueueFamilyIndices(VkPhysicalDevice physical_device,
                                                 const DummyWindow& window)
        : PresentationQueueFamilyIndices(physical_device, window.surface()) {}

Win32QueueFamilyIndices::Win32QueueFamilyIndices(VkInstance instance,
                                                 VkPhysicalDevice physical_device)
        : Win32QueueFamilyIndices(physical_device, DummyWindow(instance)) {}
}  // namespace maseya::vkbase::win32