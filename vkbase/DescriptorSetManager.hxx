#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "DescriptorPoolManager.hxx"
#include "DescriptorSetLayoutManager.hxx"
#include "ManagedDescriptorSet.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
class DescriptorSetManager {
    class DescriptorPoolSizeKey {
        struct Hasher {
            std::size_t operator()(const DescriptorPoolSizeKey& obj) const noexcept;
        };

    public:
        DescriptorPoolSizeKey(
                VkDescriptorType type, std::uint32_t descriptor_count,
                std::unordered_set<VkDescriptorType> mutable_descriptor_types);

        bool operator==(const DescriptorPoolSizeKey& rhs) const noexcept;
        bool operator!=(const DescriptorPoolSizeKey& rhs) const noexcept {
            return !(*this == rhs);
        }

    private:
        VkDescriptorType type_;
        std::uint32_t descriptor_count_;
        std::unordered_set<VkDescriptorType> mutable_descriptor_types_;

        friend class DescriptorSetManager;
    };

    class DescriptorPoolKey {
        struct Hasher {
            std::size_t operator()(const DescriptorPoolKey& obj) const noexcept;
        };

    public:
        DescriptorPoolKey(const VkDescriptorPoolCreateInfo& create_info);

        bool operator==(const DescriptorPoolKey& rhs) const noexcept;
        bool operator!=(const DescriptorPoolKey& rhs) const noexcept {
            return !(*this == rhs);
        }

    private:
        VkDescriptorPoolCreateFlags flags_;
        std::unordered_set<DescriptorPoolSizeKey, DescriptorPoolSizeKey::Hasher>
                pool_sizes_;
        std::uint32_t max_inline_uniform_block_bindings_;

        friend class DescriptorSetManager;
    };

public:
    DescriptorSetManager(std::nullptr_t)
            : descriptor_set_layout_manager_(nullptr), descriptor_pool_managers_() {}

    DescriptorSetManager(VkDevice device);

    DescriptorSetManager(const DescriptorSetManager&) = delete;
    DescriptorSetManager(DescriptorSetManager&&) noexcept = default;

    DescriptorSetManager& operator=(const DescriptorSetManager&) = delete;
    DescriptorSetManager& operator=(DescriptorSetManager&&) noexcept = default;

    VkDevice device() const noexcept { return descriptor_set_layout_manager_.device(); }

    ManagedDescriptorSet allocate_descriptor_set(
            const std::vector<VkDescriptorSetLayoutBinding>&
                    descriptor_set_layout_bindings);

    ManagedDescriptorSet allocate_descriptor_set(
            const VkDescriptorSetLayoutCreateInfo& descriptor_set_layout_create_info);

private:
    DescriptorSetLayoutManager descriptor_set_layout_manager_;
    std::unordered_map<DescriptorPoolKey, DescriptorPoolManager,
                       DescriptorPoolKey::Hasher>
            descriptor_pool_managers_;
};
}  // namespace maseya::vkbase
