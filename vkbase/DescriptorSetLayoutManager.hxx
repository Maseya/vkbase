#pragma once

#include <vulkan/vulkan_core.h>

#include <unordered_map>
#include <unordered_set>

#include "DescriptorSetLayout.hxx"

namespace maseya::vkbase {
class DescriptorSetLayoutManager {
    class DescriptorSetLayoutBinding {
        struct Hasher {
            std::size_t operator()(
                    const DescriptorSetLayoutBinding& obj) const noexcept;
        };

    public:
        DescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptor_type,
                                   uint32_t descriptor_count,
                                   VkShaderStageFlags stage_flags,
                                   const VkSampler* immutable_samplers,
                                   VkDescriptorBindingFlags descriptor_binding_flags,
                                   const VkDescriptorType* mutable_descriptor_types,
                                   uint32_t mutable_descriptor_type_count);

        bool operator==(const DescriptorSetLayoutBinding& rhs) const noexcept;
        bool operator!=(const DescriptorSetLayoutBinding& rhs) const noexcept {
            return !(*this == rhs);
        }

    private:
        uint32_t binding_;
        VkDescriptorType descriptor_type_;
        uint32_t descriptor_count_;
        VkShaderStageFlags stage_flags_;
        std::unordered_set<VkSampler> immutable_samplers_;
        VkDescriptorBindingFlags descriptor_binding_flags_;
        std::unordered_set<VkDescriptorType> mutable_descriptor_types_;

        friend class DescriptorSetLayoutManager;
    };

    class DescriptorSetLayoutKey {
        struct Hasher {
            std::size_t operator()(const DescriptorSetLayoutKey& obj) const noexcept;
        };

    public:
        DescriptorSetLayoutKey(const VkDescriptorSetLayoutCreateInfo& create_info);

        bool operator==(const DescriptorSetLayoutKey& rhs) const noexcept;
        bool operator!=(const DescriptorSetLayoutKey& rhs) const noexcept {
            return !(*this == rhs);
        }

    private:
        VkDescriptorSetLayoutCreateFlags flags_;
        std::unordered_set<DescriptorSetLayoutBinding,
                           DescriptorSetLayoutBinding::Hasher>
                descriptor_set_layout_bindings_;


        friend class DescriptorSetLayoutManager;
    };

public:
    DescriptorSetLayoutManager(std::nullptr_t)
            : device_(nullptr), descriptor_set_layouts_() {}

    DescriptorSetLayoutManager(VkDevice device);

    DescriptorSetLayoutManager(const DescriptorSetLayoutManager&) = delete;
    DescriptorSetLayoutManager(DescriptorSetLayoutManager&&) = default;

    DescriptorSetLayoutManager& operator=(const DescriptorSetLayoutManager&) = delete;
    DescriptorSetLayoutManager& operator=(DescriptorSetLayoutManager&&) = default;

    const DescriptorSetLayout& get_descriptor_set_layout(
            const std::vector<VkDescriptorSetLayoutBinding>& bindings);

    const DescriptorSetLayout& get_descriptor_set_layout(
            const VkDescriptorSetLayoutCreateInfo& create_info);

    VkDevice device() const noexcept { return device_; }

    void erase(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

    void erase(const VkDescriptorSetLayoutCreateInfo& create_info);

    void clear();

private:
    VkDevice device_;
    std::unordered_map<DescriptorSetLayoutKey, DescriptorSetLayout,
                       DescriptorSetLayoutKey::Hasher>
            descriptor_set_layouts_;
};
}  // namespace maseya::vkbase
