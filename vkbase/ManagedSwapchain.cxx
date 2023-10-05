#include "ManagedSwapchain.hxx"

#include "SwapchainSupportDetails.hxx"

namespace maseya::vkbase {
ManagedSwapchain::ManagedSwapchain(VkPhysicalDevice physical_device, VkDevice device,
                                   VkCommandPool command_pool, VkSurfaceKHR surface)
        : physical_device_(physical_device),
          device_(device),
          surface_(surface),
          swapchain_(nullptr),
          frames_(),
          current_frame_(0),
          next_frame_(0),
          old_swapchains_() {
    for (uint32_t i = 0; i < 2; i++) {
        frames_.emplace_back(device, command_pool);
    }

    recreate_swapchain();
}

std::optional<uint32_t> ManagedSwapchain::acquire_next_image(uint64_t timeout) {
    if (!swapchain_) {
        return std::nullopt;
    }

    Frame& frame = frames_[next_frame_];
    std::optional<uint32_t> result =
            frame.acquire_swapchain_image(*swapchain_, timeout);
    if (result.has_value()) {
        current_frame_ = next_frame_;
        next_frame_ = (current_frame_ + 1) % max_frames_in_flight();
    }

    return result;
}

bool ManagedSwapchain::recreate_swapchain() {
    remove_swapchain();

    VkSwapchainKHR old_swapchain =
            old_swapchains_.empty() ? VK_NULL_HANDLE : *old_swapchains_.back();

    SwapchainSupportDetails support_details(physical_device_, surface_);
    if (!support_details.is_supported()) {
        return false;
    }

    // We wait for the frames to finish any submitted commands before destroying the
    // swapchain since a recreated swapchain usually means other frame-dependent
    // resources will be recreated as well, like framebuffers.
    std::vector<VkFence> fences;
    for (const Frame& frame : frames_) {
        fences.push_back(frame.command_signal_fence());
    }
    wait_for_fences(device_, fences);

    swapchain_ = Swapchain(device_, support_details, old_swapchain);
    return true;
}

void ManagedSwapchain::remove_swapchain() {
    if (*swapchain_) {
        old_swapchains_.push(std::move(swapchain_));
    }

    // HACK: Start killing old swapchains once the queue gets large enough. The size
    // limit is completely arbitrary and there really isn't a good way to know when old
    // swapchains are done since we don't get fences that signal when a swapchain image
    // is done being presented. Realistically, we should never need a size limit larger
    // than maybe 3, but it's good to be cautious I guess. At least until this breaks
    // something. Alternatively, I can choose to never kill old swapchains and wait
    // until the surface is destroyed, but every time the user resizes a window, a new
    // swapchain is created, so this can leave us with thousands of retired swapchains.
    while (old_swapchains_.size() > 5) {
        old_swapchains_.pop();
    }
}
}  // namespace maseya::vkbase