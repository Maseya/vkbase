#pragma once

#include "DescriptorPoolSetAllocation.hxx"
#include "DescriptorSet.hxx"

namespace maseya::vkbase {
class ManagedDescriptorSet : public DescriptorSet {
public:
    ManagedDescriptorSet(std::nullptr_t) noexcept
            : descriptor_pool_set_allocation_(nullptr), DescriptorSet(nullptr) {}

    ManagedDescriptorSet(DescriptorPoolSetAllocation&& descriptor_pool_set_allocation,
                         VkDescriptorSetLayout descriptor_set_layout);

    ManagedDescriptorSet(const ManagedDescriptorSet&) = delete;
    ManagedDescriptorSet(ManagedDescriptorSet&&) noexcept = default;

    ManagedDescriptorSet& operator=(const ManagedDescriptorSet&) = delete;
    ManagedDescriptorSet& operator=(ManagedDescriptorSet&&) noexcept = default;

private:
    DescriptorPoolSetAllocation descriptor_pool_set_allocation_;
};
}  // namespace maseya::vkbase
