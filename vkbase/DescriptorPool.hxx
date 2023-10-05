#pragma once

#include <vulkan/vulkan_core.h>

#include <memory>

#include "UniqueObject.hxx"

namespace maseya::vkbase {
class DescriptorPool {
    struct Destroyer {
        constexpr Destroyer() noexcept : device(nullptr) {}

        Destroyer(VkDevice device) noexcept;

        void operator()(VkDescriptorPool descriptor_pool) const noexcept;

        VkDevice device;
    };

public:
    constexpr DescriptorPool(std::nullptr_t) noexcept : descriptor_pool_(nullptr) {}

    DescriptorPool(VkDevice device, const std::vector<VkDescriptorType>& descriptors,
                   uint32_t max_sets = 1);
    DescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo& create_info);

    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool(DescriptorPool&&) noexcept = default;

    DescriptorPool& operator=(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&&) noexcept = default;

    VkDescriptorPool operator*() const noexcept { return *descriptor_pool_; }

    explicit operator bool() const noexcept { static_cast<bool>(descriptor_pool_); }

private:
    UniqueObject<VkDescriptorPool, Destroyer> descriptor_pool_;
};
}  // namespace maseya::vkbase
