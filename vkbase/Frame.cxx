#include "Frame.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
Frame::Frame(VkDevice device, VkCommandPool command_pool)
        : device_(device),
          image_available_semaphore_(device),
          image_available_fence_(device, VK_FENCE_CREATE_SIGNALED_BIT),
          command_signal_semaphore_(device),
          command_signal_fence_(device, VK_FENCE_CREATE_SIGNALED_BIT),
          command_buffer_(device, command_pool) {}

Frame::~Frame() {
    if (command_buffer_) {
        wait();
    }
}

bool Frame::wait_for_image(uint64_t timeout) const {
    return wait_for_fence(device_, *image_available_fence_, timeout);
}

bool Frame::wait_for_command(uint64_t timeout) const {
    return wait_for_fence(device_, *command_signal_fence_, timeout);
}

std::optional<uint32_t> Frame::acquire_swapchain_image(VkSwapchainKHR swapchain,
                                                       uint64_t timeout) {
    if (!wait(timeout)) {
        return std::nullopt;
    }

    reset_fence(device_, *image_available_fence_);

    uint32_t image_index;
    VkResult result =
            vkAcquireNextImageKHR(device_, swapchain, 0, *image_available_semaphore_,
                                  *image_available_fence_, &image_index);
    switch (result) {
        case VK_ERROR_OUT_OF_DATE_KHR:
        case VK_SUBOPTIMAL_KHR:
            return std::nullopt;
        default:
            assert_result(result);
            break;
    }

    return image_index;
}

void Frame::submit(VkQueue queue) const {
    VkCommandBuffer command_buffer = *command_buffer_;
    VkSemaphore signal_semaphore = *command_signal_semaphore_;
    VkSemaphore wait_semaphore = *image_available_semaphore_;
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &wait_semaphore;
    submit_info.pWaitDstStageMask = &wait_stage;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &signal_semaphore;

    reset_fence(device_, *command_signal_fence_);
    assert_result(vkQueueSubmit(queue, 1, &submit_info, *command_signal_fence_));
}

bool Frame::wait(uint64_t timeout) const {
    VkFence fences[] = {*image_available_fence_, *command_signal_fence_};
    return wait_for_fences(device_, fences);
}
}  // namespace maseya::vkbase