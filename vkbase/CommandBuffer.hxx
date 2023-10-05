#pragma once

#include <vulkan/vulkan_core.h>

#include <glm/vec4.hpp>

#include "DescriptorSet.hxx"

namespace maseya::vkbase {
class CommandBuffer {
    struct Freer {
        constexpr Freer() noexcept : device(nullptr), command_pool(VK_NULL_HANDLE) {}

        Freer(VkDevice device, VkCommandPool command_pool) noexcept;

        void operator()(VkCommandBuffer command_buffer) const noexcept;

        VkDevice device;
        VkCommandPool command_pool;
    };

public:
    constexpr CommandBuffer(std::nullptr_t) : command_buffer_(nullptr) {}

    CommandBuffer(VkDevice device, VkCommandPool command_pool);

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&&) noexcept = default;

    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&&) noexcept = default;

    VkCommandBuffer operator*() const noexcept { return *command_buffer_; }

    explicit operator bool() const noexcept {
        return static_cast<bool>(command_buffer_);
    }

    void begin(bool one_time_submit = false) const;

    void set_viewport(const VkExtent2D& extent) const noexcept;

    void set_scissor(const VkExtent2D& extent) const noexcept;

    void set_viewport_and_scissor(const VkExtent2D& extent) const noexcept;

    void begin_flat_render_pass(VkRenderPass flat_render_pass,
                                VkFramebuffer framebuffer, const VkExtent2D& extent,
                                const glm::vec4& clear_color) const noexcept;

    void bind_graphics_pipeline(VkPipeline pipeline) const noexcept;

    void bind_descriptor_set(VkPipelineLayout pipeline_layout,
                             VkDescriptorSet descriptor_set) const noexcept;

    void draw_quads(uint32_t instance_count, uint32_t start_index = 0) const noexcept {
        draw(4, instance_count, 0, start_index);
    }

    void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t start_vertex = 0,
              uint32_t start_index = 0) const noexcept;

    void end_render_pass() const noexcept;

    void end() const;

private:
    UniqueObject<VkCommandBuffer, Freer> command_buffer_;
};
}  // namespace maseya::vkbase
