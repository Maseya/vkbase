#pragma once

#include <Windows.h>

#include "Surface.hxx"

namespace maseya::vkbase::win32 {
class Win32SurfaceFactory {
public:
    Win32SurfaceFactory(VkInstance instance);

    Surface create_surface(HWND hwnd) const;

private:
    VkInstance instance_;
};
}  // namespace maseya::vkbase::win32