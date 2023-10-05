#include "CommandBuffer.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
CommandBuffer::Freer::Freer(VkDevice device, VkCommandPool command_pool) noexcept
        : device(device), command_pool(command_pool) {}

void CommandBuffer::Freer::operator()(VkCommandBuffer command_buffer) const noexcept {
    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

CommandBuffer::CommandBuffer(VkDevice device, VkCommandPool command_pool)
        : command_buffer_(VK_NULL_HANDLE, device, command_pool) {
    command_buffer_.reset(allocate_command_buffer(device, command_pool));
}

void CommandBuffer::begin(bool one_time_submit) const {
    begin_command(*command_buffer_,
                  one_time_submit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0);
}

void CommandBuffer::set_viewport(const VkExtent2D& extent) const noexcept {
    VkViewport viewport{};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(*command_buffer_, 0, 1, &viewport);
}

void CommandBuffer::set_scissor(const VkExtent2D& extent) const noexcept {
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;
    vkCmdSetScissor(*command_buffer_, 0, 1, &scissor);
}

void CommandBuffer::set_viewport_and_scissor(const VkExtent2D& extent) const noexcept {
    set_viewport(extent);
    set_scissor(extent);
}

void CommandBuffer::begin_flat_render_pass(
        VkRenderPass flat_render_pass, VkFramebuffer framebuffer,
        const VkExtent2D& extent, const glm::vec4& clear_color) const noexcept {
    VkRenderPassBeginInfo render_pass_begin_info{};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = flat_render_pass;
    render_pass_begin_info.framebuffer = framebuffer;
    render_pass_begin_info.renderArea.offset = {0, 0};
    render_pass_begin_info.renderArea.extent = extent;
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues =
            reinterpret_cast<const VkClearValue*>(&clear_color);

    vkCmdBeginRenderPass(*command_buffer_, &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::bind_graphics_pipeline(VkPipeline pipeline) const noexcept {
    vkCmdBindPipeline(*command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void CommandBuffer::bind_descriptor_set(VkPipelineLayout pipeline_layout,
                                        VkDescriptorSet descriptor_set) const noexcept {
    vkCmdBindDescriptorSets(*command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline_layout, 0, 1, &descriptor_set, 0, nullptr);
}

void CommandBuffer::draw(uint32_t vertex_count, uint32_t instance_count,
                         uint32_t start_vertex, uint32_t start_index) const noexcept {
    vkCmdDraw(*command_buffer_, vertex_count, instance_count, start_vertex,
              start_index);
}

void CommandBuffer::end_render_pass() const noexcept {
    vkCmdEndRenderPass(*command_buffer_);
}

void CommandBuffer::end() const { end_command(*command_buffer_); }
}  // namespace maseya::vkbase