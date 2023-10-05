#include "Surface.hxx"

namespace maseya::vkbase {
Surface::Destroyer::Destroyer(VkInstance instance) noexcept : instance(instance) {}

void Surface::Destroyer::operator()(VkSurfaceKHR surface) const noexcept {
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

Surface::Surface(VkInstance instance, VkSurfaceKHR surface)
        : surface_(surface, instance) {}
}  // namespace maseya::vkbase