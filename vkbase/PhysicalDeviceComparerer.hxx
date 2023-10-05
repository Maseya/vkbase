#pragma once

#include <vulkan/vulkan.h>

namespace maseya::vkbase {
class PhysicaDeviceComparer {
public:
    PhysicaDeviceComparer(VkInstance instance, VkSurfaceKHR surface);

    bool operator()(VkPhysicalDevice lhs, VkPhysicalDevice rhs) const;

    std::uint32_t score_physical_device(VkPhysicalDevice physical_device) const;

private:
    VkInstance instance_;
    VkSurfaceKHR surface_;
};
}  // namespace maseya::vkbase
