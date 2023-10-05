#include "DescriptorSetLayoutManager.hxx"

#include <algorithm>

#include "math_helper.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
std::size_t DescriptorSetLayoutManager::DescriptorSetLayoutBinding::Hasher::operator()(
        const DescriptorSetLayoutBinding& obj) const noexcept {
    std::size_t result;
    hash_combine(result, obj.binding_);
    hash_combine(result, obj.descriptor_type_);
    hash_combine(result, obj.descriptor_count_);
    hash_combine(result, obj.stage_flags_);
    hash_combine_invariant(result, obj.immutable_samplers_);
    hash_combine(result, obj.descriptor_binding_flags_);
    hash_combine_invariant(result, obj.mutable_descriptor_types_);
    return result;
}

DescriptorSetLayoutManager::DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(
        uint32_t binding, VkDescriptorType descriptor_type, uint32_t descriptor_count,
        VkShaderStageFlags stage_flags, const VkSampler* immutable_samplers,
        VkDescriptorBindingFlags descriptor_binding_flags,
        const VkDescriptorType* mutable_descriptor_types,
        uint32_t mutable_descriptor_type_count)
        : binding_(binding),
          descriptor_type_(descriptor_type),
          descriptor_count_(descriptor_count),
          stage_flags_(stage_flags),
          immutable_samplers_(immutable_samplers,
                              immutable_samplers + descriptor_count),
          descriptor_binding_flags_(descriptor_binding_flags),
          mutable_descriptor_types_(
                  mutable_descriptor_types,
                  mutable_descriptor_types + mutable_descriptor_type_count) {}

bool DescriptorSetLayoutManager::DescriptorSetLayoutBinding::operator==(
        const DescriptorSetLayoutBinding& rhs) const noexcept {
    return binding_ == rhs.binding_ && descriptor_type_ == rhs.descriptor_type_ &&
           descriptor_count_ == rhs.descriptor_count_ &&
           stage_flags_ == rhs.stage_flags_ &&
           immutable_samplers_ == rhs.immutable_samplers_ &&
           descriptor_binding_flags_ == rhs.descriptor_binding_flags_ &&
           mutable_descriptor_types_ == rhs.mutable_descriptor_types_;
}

static void get_descriptor_set_layout_create_info_pnext_values(
        const void* pnext,
        const VkDescriptorSetLayoutBindingFlagsCreateInfo*&
                descriptor_set_layout_binding_flags_create_info,
        const VkMutableDescriptorTypeCreateInfoEXT*&
                mutable_descriptor_type_create_info);

DescriptorSetLayoutManager::DescriptorSetLayoutKey::DescriptorSetLayoutKey(
        const VkDescriptorSetLayoutCreateInfo& create_info)
        : flags_(create_info.flags), descriptor_set_layout_bindings_() {
    const VkDescriptorSetLayoutBindingFlagsCreateInfo*
            descriptor_set_layout_binding_flags_create_info = nullptr;
    const VkMutableDescriptorTypeCreateInfoEXT* mutable_descriptor_type_create_info =
            nullptr;
    get_descriptor_set_layout_create_info_pnext_values(
            create_info.pNext, descriptor_set_layout_binding_flags_create_info,
            mutable_descriptor_type_create_info);

    for (uint32_t i = 0; i < create_info.bindingCount; i++) {
        VkDescriptorBindingFlags descriptor_binding_flags = 0;
        if (descriptor_set_layout_binding_flags_create_info &&
            descriptor_set_layout_binding_flags_create_info->bindingCount) {
            descriptor_binding_flags =
                    descriptor_set_layout_binding_flags_create_info->pBindingFlags[i];
        }

        const VkDescriptorType* mutable_descriptor_types = nullptr;
        uint32_t mutable_descriptor_type_count = 0;
        if (mutable_descriptor_type_create_info &&
            mutable_descriptor_type_create_info->mutableDescriptorTypeListCount) {
            const VkMutableDescriptorTypeListEXT& mutable_descriptor_type_list =
                    mutable_descriptor_type_create_info->pMutableDescriptorTypeLists[i];

            mutable_descriptor_types = mutable_descriptor_type_list.pDescriptorTypes;
            mutable_descriptor_type_count =
                    mutable_descriptor_type_list.descriptorTypeCount;
        }

        const VkDescriptorSetLayoutBinding& binding = create_info.pBindings[i];
        descriptor_set_layout_bindings_.emplace(
                binding.binding, binding.descriptorType, binding.descriptorCount,
                binding.stageFlags, binding.pImmutableSamplers,
                descriptor_binding_flags, mutable_descriptor_types,
                mutable_descriptor_type_count);
    }
}

void get_descriptor_set_layout_create_info_pnext_values(
        const void* pnext,
        const VkDescriptorSetLayoutBindingFlagsCreateInfo*&
                descriptor_set_layout_binding_flags_create_info,
        const VkMutableDescriptorTypeCreateInfoEXT*&
                mutable_descriptor_type_create_info) {
    if (!pnext) {
        return;
    }

    switch (*reinterpret_cast<const VkStructureType*>(pnext)) {
        case VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO:
            descriptor_set_layout_binding_flags_create_info = reinterpret_cast<
                    const VkDescriptorSetLayoutBindingFlagsCreateInfo*>(pnext);

            get_descriptor_set_layout_create_info_pnext_values(
                    descriptor_set_layout_binding_flags_create_info->pNext,
                    descriptor_set_layout_binding_flags_create_info,
                    mutable_descriptor_type_create_info);
            break;

        case VK_STRUCTURE_TYPE_MUTABLE_DESCRIPTOR_TYPE_CREATE_INFO_EXT:
            mutable_descriptor_type_create_info =
                    reinterpret_cast<const VkMutableDescriptorTypeCreateInfoEXT*>(
                            pnext);

            get_descriptor_set_layout_create_info_pnext_values(
                    mutable_descriptor_type_create_info->pNext,
                    descriptor_set_layout_binding_flags_create_info,
                    mutable_descriptor_type_create_info);
            break;
    }
}

std::size_t DescriptorSetLayoutManager::DescriptorSetLayoutKey::Hasher::operator()(
        const DescriptorSetLayoutKey& obj) const noexcept {
    std::size_t result = 0;
    hash_combine(result, obj.flags_);
    hash_combine_invariant(result, obj.descriptor_set_layout_bindings_);
    return result;
}

bool DescriptorSetLayoutManager::DescriptorSetLayoutKey::operator==(
        const DescriptorSetLayoutKey& rhs) const noexcept {
    return flags_ == rhs.flags_ &&
           descriptor_set_layout_bindings_ == rhs.descriptor_set_layout_bindings_;
}

DescriptorSetLayoutManager::DescriptorSetLayoutManager(VkDevice device)
        : device_(device) {}

const DescriptorSetLayout& DescriptorSetLayoutManager::get_descriptor_set_layout(
        const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
    return get_descriptor_set_layout(get_descriptor_set_layout_create_info(bindings));
}

const DescriptorSetLayout& DescriptorSetLayoutManager::get_descriptor_set_layout(
        const VkDescriptorSetLayoutCreateInfo& create_info) {
    DescriptorSetLayoutKey key(create_info);
    auto it = descriptor_set_layouts_.find(key);
    if (it != descriptor_set_layouts_.end()) {
        return it->second;
    }

    auto result = descriptor_set_layouts_.emplace(
            create_info, DescriptorSetLayout(device_, create_info));
    return result.first->second;
}

void DescriptorSetLayoutManager::erase(
        const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
    erase(get_descriptor_set_layout_create_info(bindings));
}

void DescriptorSetLayoutManager::erase(
        const VkDescriptorSetLayoutCreateInfo& create_info) {
    descriptor_set_layouts_.erase(DescriptorSetLayoutKey(create_info));
}

void DescriptorSetLayoutManager::clear() { descriptor_set_layouts_.clear(); }
}  // namespace maseya::vkbase