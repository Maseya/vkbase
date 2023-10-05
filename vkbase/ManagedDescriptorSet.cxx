#include "ManagedDescriptorSet.hxx"

namespace maseya::vkbase {
ManagedDescriptorSet::ManagedDescriptorSet(
        DescriptorPoolSetAllocation&& descriptor_pool_set_allocation,
        VkDescriptorSetLayout descriptor_set_layout)
        : DescriptorSet(descriptor_pool_set_allocation.device(),
                        descriptor_pool_set_allocation.descriptor_pool(),
                        descriptor_set_layout, true),
          descriptor_pool_set_allocation_(std::move(descriptor_pool_set_allocation)) {}
}  // namespace maseya::vkbase