#pragma once

#include <cstddef>
#include <vector>

#include "DescriptorPoolManager.hxx"
#include "DescriptorSetLayout.hxx"
#include "UniqueObject.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
class Pipeline {
    struct Destroyer {
        constexpr Destroyer() noexcept : device(nullptr) {}

        Destroyer(VkDevice device) noexcept;

        void operator()(VkPipeline pipeline) const noexcept;

        VkDevice device;
    };

public:
    Pipeline(std::nullptr_t) noexcept;

    Pipeline(VkDevice device, VkRenderPass render_pass,
             const std::vector<VkDescriptorSetLayoutBinding>&
                     descriptor_set_layout_bindings,
             VkShaderModule vertex_shader, VkShaderModule fragment_shader)
            : Pipeline(device, render_pass, descriptor_set_layout_bindings.data(),
                       static_cast<uint32_t>(descriptor_set_layout_bindings.size()),
                       vertex_shader, fragment_shader) {}
    template <size_t descriptor_set_layout_binding_count>
    Pipeline(VkDevice device, VkRenderPass render_pass,
             const VkDescriptorSetLayoutBinding (&descriptor_set_layout_bindings)
                     [descriptor_set_layout_binding_count],
             VkShaderModule vertex_shader, VkShaderModule fragment_shader)
            : Pipeline(device, render_pass, descriptor_set_layout_bindings,
                       static_cast<uint32_t>(descriptor_set_layout_binding_count),
                       vertex_shader, fragment_shader) {}
    Pipeline(VkDevice device, VkRenderPass render_pass,
             const VkDescriptorSetLayoutBinding* descriptor_set_layout_bindings,
             uint32_t descriptor_set_layout_binding_count, VkShaderModule vertex_shader,
             VkShaderModule fragment_shader)
            : Pipeline(device, render_pass, descriptor_set_layout_bindings,
                       descriptor_set_layout_binding_count, vertex_shader,
                       fragment_shader, nullptr, 0, nullptr, 0) {}

    Pipeline(VkDevice device, VkRenderPass render_pass,
             const std::vector<VkDescriptorSetLayoutBinding>&
                     descriptor_set_layout_bindings,
             VkShaderModule vertex_shader, VkShaderModule fragment_shader,
             const std::vector<VkVertexInputBindingDescription>&
                     vertex_binding_descriptions,
             const std::vector<VkVertexInputAttributeDescription>&
                     vertex_attribute_descriptions)
            : Pipeline(device, render_pass, descriptor_set_layout_bindings.data(),
                       static_cast<uint32_t>(descriptor_set_layout_bindings.size()),
                       vertex_shader, fragment_shader,
                       vertex_binding_descriptions.data(),
                       static_cast<uint32_t>(vertex_binding_descriptions.size()),
                       vertex_attribute_descriptions.data(),
                       static_cast<uint32_t>(vertex_attribute_descriptions.size())) {}
    template <size_t descriptor_set_layout_binding_count>
    Pipeline(VkDevice device, VkRenderPass render_pass,
             const VkDescriptorSetLayoutBinding (&descriptor_set_layout_bindings)
                     [descriptor_set_layout_binding_count],
             VkShaderModule vertex_shader, VkShaderModule fragment_shader,
             const std::vector<VkVertexInputBindingDescription>&
                     vertex_binding_descriptions,
             const std::vector<VkVertexInputAttributeDescription>&
                     vertex_attribute_descriptions)
            : Pipeline(device, render_pass, descriptor_set_layout_bindings,
                       static_cast<uint32_t>(descriptor_set_layout_binding_count),
                       vertex_shader, fragment_shader,
                       vertex_binding_descriptions.data(),
                       static_cast<uint32_t>(vertex_binding_descriptions.size()),
                       vertex_attribute_descriptions.data(),
                       static_cast<uint32_t>(vertex_attribute_descriptions.size())) {}
    template <size_t descriptor_set_layout_binding_count,
              size_t vertex_binding_description_count,
              size_t vertex_attribute_description_count>
    Pipeline(
            VkDevice device, VkRenderPass render_pass,
            const VkDescriptorSetLayoutBinding (&descriptor_set_layout_bindings)
                    [descriptor_set_layout_binding_count],
            VkShaderModule vertex_shader, VkShaderModule fragment_shader,
            const VkVertexInputBindingDescription (
                    &vertex_binding_descriptions)[vertex_binding_description_count],
            const VkVertexInputAttributeDescription (
                    &vertex_attribute_descriptions)[vertex_attribute_description_count])
            : Pipeline(device, render_pass, descriptor_set_layout_bindings,
                       static_cast<uint32_t>(descriptor_set_layout_binding_count),
                       vertex_shader, fragment_shader, vertex_binding_descriptions,
                       static_cast<uint32_t>(vertex_binding_description_count),
                       vertex_attribute_descriptions,
                       static_cast<uint32_t>(vertex_attribute_description_count)) {}
    Pipeline(VkDevice device, VkRenderPass render_pass,
             const VkDescriptorSetLayoutBinding* descriptor_set_layout_bindings,
             uint32_t descriptor_set_layout_binding_count, VkShaderModule vertex_shader,
             VkShaderModule fragment_shader,
             const VkVertexInputBindingDescription* vertex_binding_descriptions,
             uint32_t vertex_binding_description_count,
             const VkVertexInputAttributeDescription* vertex_attribute_descriptions,
             uint32_t vertex_attribute_description_count);

    Pipeline(const Pipeline&) = delete;
    Pipeline(Pipeline&&) noexcept = default;

    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&&) noexcept = default;

    VkPipeline operator*() const noexcept { return *pipeline_; }

    operator bool() const noexcept { return static_cast<bool>(pipeline_); }

    DescriptorPoolManager create_descriptor_pool_manager() const;

private:
    UniqueObject<VkPipeline, Destroyer> pipeline_;
};
}  // namespace maseya::vkbase
