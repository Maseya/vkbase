#include "DescriptorSet.hxx"

#include <unordered_map>

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
DescriptorSet::Freer::Freer(VkDevice device, VkDescriptorPool descriptor_pool) noexcept
        : device(device), descriptor_pool(descriptor_pool) {}

void DescriptorSet::Freer::operator()(VkDescriptorSet descriptor_set) const noexcept {
    // HACK(nrg): descriptor_pool will not be set when it is a pool that does not allow
    // freeing individual descriptor sets.
    if (descriptor_pool) {
        vkFreeDescriptorSets(device, descriptor_pool, 1, &descriptor_set);
    }
}

DescriptorSet::DescriptorSet(VkDevice device, VkDescriptorPool descriptor_pool,
                             VkDescriptorSetLayout descriptor_set_layout, bool free_bit)
        : device_(device),
          descriptor_set_(VK_NULL_HANDLE, device,
                          free_bit ? descriptor_pool : VK_NULL_HANDLE) {
    descriptor_set_.reset(
            create_descriptor_set(device, descriptor_pool, descriptor_set_layout));
}

void DescriptorSet::write(VkBuffer buffer, VkDescriptorType descriptor_type,
                          VkDeviceSize offset, VkDeviceSize size,
                          uint32_t binding) const {
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = buffer;
    buffer_info.offset = offset;
    buffer_info.range = size;

    VkWriteDescriptorSet descriptor_write{};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = *descriptor_set_;

    descriptor_write.dstBinding = binding;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = descriptor_type;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(device_, 1, &descriptor_write, 0, NULL);
}

void DescriptorSet::write(VkImageView image_view, VkSampler sampler,
                          uint32_t binding) const {
    VkDescriptorImageInfo image_info{};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = image_view;
    image_info.sampler = sampler;

    VkWriteDescriptorSet descriptor_write{};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = *descriptor_set_;
    descriptor_write.dstBinding = binding;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pImageInfo = &image_info;

    vkUpdateDescriptorSets(device_, 1, &descriptor_write, 0, nullptr);
}

VkDescriptorType DescriptorSet::get_descriptor_type(VkBufferUsageFlags usage) {
    static const std::unordered_map<VkBufferUsageFlags, VkDescriptorType> options = {
            {VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
             VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER},
            {VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
             VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER},
            {VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER},
            {VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},
    };

    constexpr static VkBufferUsageFlags base_usage =
            VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT |
            VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT |
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    auto it = options.find(usage & base_usage);
    if (it == options.end()) {
        return {};
    }

    return it->second;
}
}  // namespace maseya::vkbase