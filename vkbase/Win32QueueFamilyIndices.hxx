#pragma once

#include "DummyWindow.hxx"
#include "PresentationQueueFamilyIndices.hxx"

namespace maseya::vkbase::win32 {
class Win32QueueFamilyIndices : public PresentationQueueFamilyIndices {
private:
    // TODO(nrg): Surface gets temporarily created and destroyed, which enforces a
    // restriction in the base class that VkSurfaceKHR not be retained. While that's
    // fine, I don't like the situation. Think of alternatives that read easier.
    Win32QueueFamilyIndices(VkPhysicalDevice physical_device,
                            const DummyWindow& window);

public:
    Win32QueueFamilyIndices(VkInstance instance, VkPhysicalDevice physical_device);
};
}  // namespace maseya::vkbase::win32
