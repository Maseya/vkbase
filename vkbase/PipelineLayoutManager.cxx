#include "PipelineLayoutManager.hxx"

#include "math_helper.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
std::size_t PipelineLayoutManager::PipelineLayoutKey::Hasher::operator()(
        const PipelineLayoutKey& obj) const noexcept {
    std::size_t result = 0;
    hash_combine(result, obj.flags_);
    hash_combine_invariant(result, obj.descriptor_sets_);
    hash_combine_invariant(result, obj.push_constant_ranges_);
    return result;
}

std::size_t
PipelineLayoutManager::PipelineLayoutKey::PushConstantRangeHasher::operator()(
        const VkPushConstantRange& obj) const noexcept {
    std::size_t result = 0;
    hash_combine(result, obj.stageFlags);
    hash_combine(result, obj.offset);
    hash_combine(result, obj.size);
    return result;
}

bool PipelineLayoutManager::PipelineLayoutKey::PushConstantRangeEq::operator()(
        const VkPushConstantRange& lhs, const VkPushConstantRange& rhs) const noexcept {
    return lhs.stageFlags == rhs.stageFlags && lhs.offset == rhs.offset &&
           lhs.size == rhs.size;
}

PipelineLayoutManager::PipelineLayoutKey::PipelineLayoutKey(
        const VkPipelineLayoutCreateInfo& create_info)
        : flags_(create_info.flags),
          descriptor_sets_(create_info.pSetLayouts,
                           create_info.pSetLayouts + create_info.setLayoutCount),
          push_constant_ranges_(create_info.pPushConstantRanges,
                                create_info.pPushConstantRanges +
                                        create_info.pushConstantRangeCount) {}

bool PipelineLayoutManager::PipelineLayoutKey::operator==(
        const PipelineLayoutKey& rhs) const noexcept {
    return flags_ == rhs.flags_ && descriptor_sets_ == rhs.descriptor_sets_ &&
           push_constant_ranges_ == rhs.push_constant_ranges_;
}

PipelineLayoutManager::PipelineLayoutManager(VkDevice device)
        : device_(device), pipeline_layouts_() {}

const PipelineLayout& PipelineLayoutManager::get_pipeline_layout(
        const VkDescriptorSetLayout* descriptor_set_layouts, std::uint32_t count) {
    return get_pipeline_layout(
            get_pipeline_layout_create_info(descriptor_set_layouts, count));
}

const PipelineLayout& PipelineLayoutManager::get_pipeline_layout(
        VkDescriptorSetLayout descriptor_set_layout) {
    return get_pipeline_layout(&descriptor_set_layout, 1);
}

const PipelineLayout& PipelineLayoutManager::get_pipeline_layout(
        const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts) {
    return get_pipeline_layout(
            descriptor_set_layouts.data(),
            static_cast<std::uint32_t>(descriptor_set_layouts.size()));
}

const PipelineLayout& PipelineLayoutManager::get_pipeline_layout(
        const VkPipelineLayoutCreateInfo& create_info) {
    PipelineLayoutKey key(create_info);
    auto it = pipeline_layouts_.find(key);
    if (it != pipeline_layouts_.end()) {
        return it->second;
    }

    auto it2 = pipeline_layouts_.insert(
            {std::move(key), PipelineLayout(device_, create_info)});
    return it2.first->second;
}
}  // namespace maseya::vkbase