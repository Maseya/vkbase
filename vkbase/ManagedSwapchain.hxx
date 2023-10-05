#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>
#include <queue>

#include "Frame.hxx"
#include "Swapchain.hxx"

namespace maseya::vkbase {
class ManagedSwapchain {
public:
    ManagedSwapchain(VkPhysicalDevice physical_device, VkDevice device,
                     VkCommandPool command_pool, VkSurfaceKHR surface);
    ManagedSwapchain(const ManagedSwapchain&) = delete;
    ManagedSwapchain(ManagedSwapchain&&) noexcept = default;

    ManagedSwapchain& operator=(const ManagedSwapchain&) = delete;
    ManagedSwapchain& operator=(ManagedSwapchain&&) noexcept = default;

    VkSurfaceKHR surface() const noexcept { return surface_; }

    const Swapchain& operator*() const noexcept { return swapchain_; }
    Swapchain& operator*() { return swapchain_; }

    const Swapchain* operator->() const noexcept { return &swapchain_; }
    Swapchain* operator->() { return &swapchain_; }

    const std::vector<SwapchainImage>& images() const noexcept {
        return swapchain_.images();
    }

    uint32_t max_frames_in_flight() const noexcept {
        return static_cast<uint32_t>(frames_.size());
    }

    Frame& current_frame() { return frames_[current_frame_]; }
    const Frame& current_frame() const { return frames_[current_frame_]; }

    std::optional<uint32_t> acquire_next_image(uint64_t timeout = UINT64_MAX);

    bool recreate_swapchain();

    void remove_swapchain();

private:
    VkPhysicalDevice physical_device_;
    VkDevice device_;
    VkSurfaceKHR surface_;

    Swapchain swapchain_;
    std::vector<Frame> frames_;
    uint32_t current_frame_;
    uint32_t next_frame_;

    std::queue<Swapchain> old_swapchains_;
};
}  // namespace maseya::vkbase
