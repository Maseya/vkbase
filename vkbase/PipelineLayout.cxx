#include "PipelineLayout.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
PipelineLayout::Destroyer::Destroyer(VkDevice device) : device(device) {}

void PipelineLayout::Destroyer::operator()(
        VkPipelineLayout pipeline_layout) const noexcept {
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
}

PipelineLayout::PipelineLayout(VkDevice device,
                               const VkDescriptorSetLayout* descriptor_set_layouts,
                               std::uint32_t count)
        : PipelineLayout(device, get_pipeline_layout_create_info(descriptor_set_layouts,
                                                                 count)) {}

PipelineLayout::PipelineLayout(VkDevice device,
                               VkDescriptorSetLayout descriptor_set_layout)
        : PipelineLayout(device, &descriptor_set_layout, 1) {}

PipelineLayout::PipelineLayout(
        VkDevice device,
        const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts)
        : PipelineLayout(device, descriptor_set_layouts.data(),
                         static_cast<std::uint32_t>(descriptor_set_layouts.size())) {}

PipelineLayout::PipelineLayout(VkDevice device,
                               const VkPipelineLayoutCreateInfo& create_info)
        : pipeline_layout_(VK_NULL_HANDLE, device) {
    create_pipeline_layout(device, create_info);
}
}  // namespace maseya::vkbase