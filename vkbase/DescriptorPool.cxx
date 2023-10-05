#include "DescriptorPool.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
DescriptorPool::Destroyer::Destroyer(VkDevice device) noexcept : device(device) {}

void DescriptorPool::Destroyer::operator()(
        VkDescriptorPool descriptor_pool) const noexcept {
    vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
}

DescriptorPool::DescriptorPool(VkDevice device,
                               const std::vector<VkDescriptorType>& descriptors,
                               uint32_t max_sets)
        : DescriptorPool(device,
                         get_descriptor_pool_create_info(descriptors, max_sets)) {}

DescriptorPool::DescriptorPool(VkDevice device,
                               const VkDescriptorPoolCreateInfo& create_info)
        : descriptor_pool_(VK_NULL_HANDLE, device) {
    descriptor_pool_.reset(create_descriptor_pool(device, create_info));
}
}  // namespace maseya::vkbase