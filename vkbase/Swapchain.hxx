#pragma once

#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <vector>

#include "SwapchainImage.hxx"
#include "SwapchainSupportDetails.hxx"
#include "UniqueObject.hxx"

namespace maseya::vkbase {
class Swapchain {
    struct Destroyer {
        constexpr Destroyer() noexcept : device(nullptr) {}

        Destroyer(VkDevice device) noexcept;

        void operator()(VkSwapchainKHR swapchain) const noexcept;

        VkDevice device;
    };

public:
    Swapchain(std::nullptr_t) noexcept : swapchain_(nullptr), images_() {}

    Swapchain(VkDevice device, const SwapchainSupportDetails& support_details,
              VkSwapchainKHR old_swapchain = VK_NULL_HANDLE);

    Swapchain(const Swapchain&) = delete;
    Swapchain(Swapchain&&) noexcept = default;

    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&) noexcept = default;

    VkSwapchainKHR operator*() const noexcept { return *swapchain_; }

    explicit operator bool() const noexcept { return static_cast<bool>(swapchain_); }

    const std::vector<SwapchainImage>& images() const { return images_; }

private:
    UniqueObject<VkSwapchainKHR, Destroyer> swapchain_;
    std::vector<SwapchainImage> images_;
};
}  // namespace maseya::vkbase