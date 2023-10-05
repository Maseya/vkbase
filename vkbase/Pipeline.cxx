#include "Pipeline.hxx"

namespace maseya::vkbase {
void Pipeline::Destroyer::operator()(VkPipeline pipeline) const noexcept {
    vkDestroyPipeline(device, pipeline, nullptr);
}

Pipeline::Destroyer::Destroyer(VkDevice device) noexcept : device(device) {}

Pipeline::Pipeline(std::nullptr_t) noexcept
        : device_(nullptr),
          bindings_(),
          descriptor_set_layout_(VK_NULL_HANDLE),
          pipeline_layout_(VK_NULL_HANDLE),
          pipeline_(VK_NULL_HANDLE) {}

Pipeline::Pipeline(
        VkDevice device, VkRenderPass render_pass,
        const VkDescriptorSetLayoutBinding* descriptor_set_layout_bindings,
        uint32_t descriptor_set_layot_binding_count, VkShaderModule vertex_shader,
        VkShaderModule fragment_shader,
        const VkVertexInputBindingDescription* vertex_binding_descriptions,
        uint32_t vertex_binding_description_count,
        const VkVertexInputAttributeDescription* vertex_attribute_descriptions,
        uint32_t vertex_attribute_description_count)
        : device_(device),
          bindings_(
                  descriptor_set_layout_bindings,
                  descriptor_set_layout_bindings + descriptor_set_layot_binding_count),
          descriptor_set_layout_(create_descriptor_set_layout(device, bindings_),
                                 device),
          pipeline_layout_(create_pipeline_layout(device, *descriptor_set_layout_),
                           device),
          pipeline_(create_graphics_pipeline(device, render_pass, *pipeline_layout_,
                                             vertex_shader, fragment_shader,
                                             vertex_binding_descriptions,
                                             vertex_binding_description_count,
                                             vertex_attribute_descriptions,
                                             vertex_attribute_description_count),
                    device) {}

DescriptorPoolManager Pipeline::create_descriptor_pool_manager() const {
    return DescriptorPoolManager(device_, *descriptor_set_layout_,
                                 get_descriptor_types(bindings_));
}
}  // namespace maseya::vkbase