#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

#include "UniqueObject.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
class DescriptorSetLayout {
    struct Destroyer {
        constexpr Destroyer() noexcept : device(nullptr) {}

        Destroyer(VkDevice device) noexcept;

        void operator()(VkDescriptorSetLayout descriptor_set_layout) const noexcept;

        VkDevice device;
    };

public:
    constexpr DescriptorSetLayout(std::nullptr_t) noexcept
            : descriptor_set_layout_(nullptr) {}

    DescriptorSetLayout(VkDevice device,
                        const VkDescriptorSetLayoutCreateInfo& create_info);

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout(DescriptorSetLayout&&) noexcept = default;

    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&) noexcept = default;

    VkDescriptorSetLayout operator*() const noexcept { return *descriptor_set_layout_; }

    explicit operator bool() const noexcept {
        return static_cast<bool>(descriptor_set_layout_);
    }

private:
    UniqueObject<VkDescriptorSetLayout, Destroyer> descriptor_set_layout_;
};
}  // namespace maseya::vkbase
