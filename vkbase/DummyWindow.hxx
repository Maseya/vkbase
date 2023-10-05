#pragma once

#include <vulkan/vulkan.h>

#include "Surface.hxx"
#include "UniqueObject.hxx"

namespace maseya::vkbase::win32 {
// To get the default surface format, we need a default surface. This little class lets
// us create a window with all default parameters for the sole purpose of creating a
// surface and then determining what the format will be.
class DummyWindow {
    struct WindowDestroyer {
        void operator()(HWND hwnd) const noexcept;
    };

public:
    constexpr DummyWindow(std::nullptr_t) noexcept
            : window_(nullptr), surface_(nullptr) {}

    DummyWindow(VkInstance instance);

    DummyWindow(const DummyWindow&) = delete;
    DummyWindow(DummyWindow&&) = default;

    DummyWindow& operator=(const DummyWindow&) = delete;
    DummyWindow& operator=(DummyWindow&&) = default;

    HWND hwnd() const noexcept { return window_.get(); }
    VkSurfaceKHR surface() const noexcept { return *surface_; }

private:
    UniqueObject<HWND, WindowDestroyer> window_;
    Surface surface_;
};
}  // namespace maseya::vkbase::win32
