#include "Win32SurfaceFactory.hxx"

#include "VulkanError.hxx"
#include "vulkan_win32_helper.hxx"

namespace maseya::vkbase::win32 {
Win32SurfaceFactory::Win32SurfaceFactory(VkInstance instance) : instance_(instance) {}

Surface Win32SurfaceFactory::create_surface(HWND hwnd) const {
    return Surface(instance_, win32::create_surface(instance_, hwnd));
}
}  // namespace maseya::vkbase::win32