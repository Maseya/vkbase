#include "DescriptorSetLayout.hxx"

namespace maseya::vkbase {
void DescriptorSetLayout::Destroyer::operator()(
        VkDescriptorSetLayout descriptor_set_layout) const noexcept {
    vkDestroyDescriptorSetLayout(device, descriptor_set_layout, nullptr);
}

DescriptorSetLayout::DescriptorSetLayout(
        VkDevice device, const VkDescriptorSetLayoutCreateInfo& create_info)
        : descriptor_set_layout_(VK_NULL_HANDLE, device) {
    descriptor_set_layout_.reset(create_descriptor_set_layout(device, create_info));
}
}  // namespace maseya::vkbase