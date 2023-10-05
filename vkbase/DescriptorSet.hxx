#pragma once

#include <vulkan/vulkan.h>

#include "Buffer.hxx"
#include "UniqueObject.hxx"

namespace maseya::vkbase {
class DescriptorSet {
    struct Freer {
        constexpr Freer() noexcept : device(nullptr), descriptor_pool(VK_NULL_HANDLE) {}

        Freer(VkDevice device, VkDescriptorPool descriptor_pool) noexcept;

        void operator()(VkDescriptorSet descriptor_set) const noexcept;

        VkDevice device;
        VkDescriptorPool descriptor_pool;
    };

public:
    DescriptorSet(std::nullptr_t) noexcept : descriptor_set_(nullptr) {}

    DescriptorSet(VkDevice device, VkDescriptorPool descriptor_pool,
                  VkDescriptorSetLayout descriptor_set_layout, bool free_bit = false);

    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet(DescriptorSet&&) noexcept = default;

    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&&) noexcept = default;

    VkDevice device() const noexcept { return device_; }
    VkDescriptorSet operator*() const noexcept { return *descriptor_set_; }

    explicit operator bool() const noexcept {
        return static_cast<bool>(descriptor_set_);
    }

    void write(VkBuffer buffer, VkDescriptorType descriptor_type, VkDeviceSize offset,
               VkDeviceSize size, uint32_t binding = 0) const;

    void write(const Buffer& buffer, uint32_t binding = 0) const {
        write(*buffer, get_descriptor_type(buffer.usage()), 0, buffer.size(), binding);
    }

    void write(VkImageView image_view, VkSampler sampler, uint32_t binding) const;

private:
    static VkDescriptorType get_descriptor_type(VkBufferUsageFlags usage);

private:
    VkDevice device_;
    UniqueObject<VkDescriptorSet, Freer> descriptor_set_;
};
}  // namespace maseya::vkbase
