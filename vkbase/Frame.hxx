#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>

#include "CommandBuffer.hxx"
#include "Fence.hxx"
#include "Semaphore.hxx"

namespace maseya::vkbase {
class Frame {
public:
    Frame(VkDevice device, VkCommandPool command_pool);

    Frame(const Frame&) = delete;
    Frame(Frame&&) noexcept = default;

    ~Frame();

    Frame& operator=(const Frame&) = delete;
    Frame& operator=(Frame&&) noexcept = default;

    VkSemaphore image_available_semaphore() const noexcept {
        return *image_available_semaphore_;
    }
    VkFence image_available_fence() const noexcept { return *image_available_fence_; }

    VkSemaphore command_signal_semaphore() const noexcept {
        return *command_signal_semaphore_;
    }
    VkFence command_signal_fence() const noexcept { return *command_signal_fence_; }

    const CommandBuffer& command_buffer() const noexcept { return command_buffer_; }

    bool wait_for_image(uint64_t timeout = UINT64_MAX) const;

    bool wait_for_command(uint64_t timeout = UINT64_MAX) const;

    std::optional<uint32_t> acquire_swapchain_image(VkSwapchainKHR swapchain,
                                                    uint64_t timeout = UINT64_MAX);

    void submit(VkQueue queue) const;

private:
    bool wait(uint64_t timeout = UINT64_MAX) const;

private:
    VkDevice device_;

    Semaphore image_available_semaphore_;
    Fence image_available_fence_;

    Semaphore command_signal_semaphore_;
    Fence command_signal_fence_;
    CommandBuffer command_buffer_;
};
}  // namespace maseya::vkbase
