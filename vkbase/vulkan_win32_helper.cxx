#include "vulkan_win32_helper.hxx"

#include "DummyWindow.hxx"
#include "SwapchainSupportDetails.hxx"
#include "VulkanError.hxx"
#include "vulkan_helper.hxx"
#include "vulkan_surface_helper.hxx"

namespace maseya::vkbase::win32 {
HINSTANCE get_hinstance(HWND hwnd) {
    return reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(hwnd, GWLP_HINSTANCE));
}

VkSurfaceKHR create_surface(VkInstance instance, HWND hwnd) {
    VkWin32SurfaceCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hwnd = hwnd;

    // TODO(nrg): Not sure if this particular HINSTANCE will always satisfy the Vulkan
    // requirement. You can get hinstances from other modules or even from HWND itself.
    // The Vulkan spec asks for "The Win32 HINSTANCE for the window..." so I'm assuming
    // that this is it. However, the WinAPI documentation says that GetWindowLongPtrW
    // "Retrieves a handle to the application instance." I'm not 100% sure this is
    // actually the HINSTANCE for the HWND though. Consider an HWND created from another
    // module. What is the correct HINSTANCE to pass in that scenario?
    create_info.hinstance = get_hinstance(hwnd);

    VkSurfaceKHR result;
    assert_result(vkCreateWin32SurfaceKHR(instance, &create_info, nullptr, &result));

    return result;
}

HWND create_dummy_hwnd() {
    int width, height;
    get_window_size_from_client(0x100, 0x100, width, height);

    HWND result = CreateWindowExW(0, L"STATIC", L"dummy", 0, 0, 0, width, height, NULL,
                                  NULL, NULL, NULL);
    if (!result) {
        throw VulkanWinApiError();
    }
    return result;
}

VkFormat get_default_format(VkInstance instance, VkPhysicalDevice physical_device) {
    DummyWindow window(instance);

    std::optional<VkSurfaceFormatKHR> surface_format =
            pick_surface_format(physical_device, window.surface());
    if (!surface_format.has_value()) {
        throw VkBaseError("Physical Device cannot present to Windows!");
    }

    return surface_format.value().format;
}

VkPhysicalDevice pick_physical_device(VkInstance instance) {
    DummyWindow window(instance);
    return maseya::vkbase::pick_physical_device(instance, window.surface());
}

void get_window_size_from_client(int client_width, int client_height, int& window_width,
                                 int& window_height) {
    int cxSizeFrame = GetSystemMetrics(SM_CXSIZEFRAME);
    int cxPadding = GetSystemMetrics(SM_CXPADDEDBORDER);

    int cySizeFrame = GetSystemMetrics(SM_CYSIZEFRAME);
    int cyCaption = GetSystemMetrics(SM_CYCAPTION);

    window_width = client_width + 2 * (cxSizeFrame + cxPadding);
    window_height = client_height + cyCaption + 2 * (cySizeFrame + cxPadding);
}
}  // namespace maseya::vkbase::win32