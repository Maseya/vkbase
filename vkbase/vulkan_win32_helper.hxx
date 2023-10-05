#pragma once

#include <vulkan/vulkan.h>

namespace maseya::vkbase::win32 {
HINSTANCE get_hinstance(HWND hwnd);

VkSurfaceKHR create_surface(VkInstance instance, HWND hwnd);

// Create an unusable, unresponsive window whose sole purpose is giving us an HWND so we
// can construct a Win32 VkSurface for testing purposes.
HWND create_dummy_hwnd();

VkFormat get_default_format(VkInstance instance, VkPhysicalDevice physical_device);

VkPhysicalDevice pick_physical_device(VkInstance instance);

void get_window_size_from_client(int client_width, int client_height, int& window_width,
                                 int& window_height);
}  // namespace maseya::vkbase::win32
