#pragma once

#include <vulkan/vulkan_core.h>

#include <unordered_map>
#include <unordered_set>

#include "PipelineLayout.hxx"

namespace maseya::vkbase {
class PipelineLayoutManager {
    class PipelineLayoutKey {
        struct Hasher {
            std::size_t operator()(const PipelineLayoutKey& obj) const noexcept;
        };

        struct PushConstantRangeHasher {
            std::size_t operator()(const VkPushConstantRange& obj) const noexcept;
        };

        struct PushConstantRangeEq {
            bool operator()(const VkPushConstantRange& lhs,
                            const VkPushConstantRange& rhs) const noexcept;
        };

    public:
        PipelineLayoutKey(const VkPipelineLayoutCreateInfo& create_info);

        bool operator==(const PipelineLayoutKey& rhs) const noexcept;
        bool operator!=(const PipelineLayoutKey& rhs) const noexcept {
            return !(*this == rhs);
        }

    private:
        VkPipelineLayoutCreateFlags flags_;
        std::unordered_set<VkDescriptorSetLayout> descriptor_sets_;
        std::unordered_set<VkPushConstantRange, PushConstantRangeHasher,
                           PushConstantRangeEq>
                push_constant_ranges_;

        friend class PipelineLayoutManager;
    };

public:
    PipelineLayoutManager(std::nullptr_t) noexcept
            : device_(nullptr), pipeline_layouts_() {}

    PipelineLayoutManager(VkDevice device);

    PipelineLayoutManager(const PipelineLayoutManager&) = delete;
    PipelineLayoutManager(PipelineLayoutManager&&) noexcept = default;

    PipelineLayoutManager& operator=(const PipelineLayoutManager&) = delete;
    PipelineLayoutManager& operator=(PipelineLayoutManager&&) noexcept = default;

    const PipelineLayout& get_pipeline_layout(
            const VkDescriptorSetLayout* descriptor_set_layouts, std::uint32_t count);
    const PipelineLayout& get_pipeline_layout(
            VkDescriptorSetLayout descriptor_set_layout);
    const PipelineLayout& get_pipeline_layout(
            const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts);
    template <std::uint32_t N>
    const PipelineLayout& get_pipeline_layout(
            const VkDescriptorSetLayout (&descriptor_set_layouts)[N]) {
        return get_pipeline_layout(descriptor_set_layouts, N);
    }

    const PipelineLayout& get_pipeline_layout(
            const VkPipelineLayoutCreateInfo& create_info);

private:
    VkDevice device_;
    std::unordered_map<PipelineLayoutKey, PipelineLayout, PipelineLayoutKey::Hasher>
            pipeline_layouts_;
};
}  // namespace maseya::vkbase