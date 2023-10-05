#include "DummyWindow.hxx"

#include "vulkan_win32_helper.hxx"

namespace maseya::vkbase::win32 {
void DummyWindow::WindowDestroyer::operator()(HWND hwnd) const noexcept {
    DestroyWindow(hwnd);
}

DummyWindow::DummyWindow(VkInstance instance)
        : window_(create_dummy_hwnd()),
          surface_(instance, create_surface(instance, window_.get())) {}
}  // namespace maseya::vkbase::win32