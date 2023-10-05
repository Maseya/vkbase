#include "DescriptorSetManager.hxx"

#include "math_helper.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
std::size_t DescriptorSetManager::DescriptorPoolSizeKey::Hasher::operator()(
        const DescriptorPoolSizeKey& obj) const noexcept {
    std::size_t result = 0;
    hash_combine(result, obj.type_);
    hash_combine(result, obj.descriptor_count_);
    hash_combine_invariant(result, obj.mutable_descriptor_types_);
    return result;
}

DescriptorSetManager::DescriptorPoolSizeKey::DescriptorPoolSizeKey(
        VkDescriptorType type, std::uint32_t descriptor_count,
        std::unordered_set<VkDescriptorType> mutable_descriptor_types)
        : type_(type),
          descriptor_count_(descriptor_count),
          mutable_descriptor_types_(mutable_descriptor_types) {}

bool DescriptorSetManager::DescriptorPoolSizeKey::operator==(
        const DescriptorPoolSizeKey& rhs) const noexcept {
    return type_ == rhs.type_ && descriptor_count_ == rhs.descriptor_count_ &&
           mutable_descriptor_types_ == rhs.mutable_descriptor_types_;
}

std::size_t DescriptorSetManager::DescriptorPoolKey::Hasher::operator()(
        const DescriptorPoolKey& obj) const noexcept {
    std::size_t result = 0;
    hash_combine(result, obj.flags_);
    hash_combine_invariant(result, obj.pool_sizes_);
    hash_combine(result, obj.max_inline_uniform_block_bindings_);
    return result;
}

static void get_descriptor_set_layout_create_info_pnext_values(
        const void* pnext,
        const VkDescriptorPoolInlineUniformBlockCreateInfo*&
                descriptor_pool_inline_uniform_block_create_info,
        const VkMutableDescriptorTypeCreateInfoEXT*&
                mutable_descriptor_type_create_info);

DescriptorSetManager::DescriptorPoolKey::DescriptorPoolKey(
        const VkDescriptorPoolCreateInfo& create_info)
        : flags_(create_info.flags),
          pool_sizes_(),
          max_inline_uniform_block_bindings_(0) {
    const VkDescriptorPoolInlineUniformBlockCreateInfo*
            descriptor_pool_inline_uniform_block_create_info = nullptr;
    const VkMutableDescriptorTypeCreateInfoEXT* mutable_descriptor_type_create_info =
            nullptr;
    get_descriptor_set_layout_create_info_pnext_values(
            create_info.pNext, descriptor_pool_inline_uniform_block_create_info,
            mutable_descriptor_type_create_info);

    if (descriptor_pool_inline_uniform_block_create_info) {
        max_inline_uniform_block_bindings_ =
                descriptor_pool_inline_uniform_block_create_info
                        ->maxInlineUniformBlockBindings;
    }

    for (std::uint32_t i = 0; i < create_info.poolSizeCount; i++) {
        std::unordered_set<VkDescriptorType> mutable_descriptor_types;
        if (mutable_descriptor_type_create_info &&
            mutable_descriptor_type_create_info->mutableDescriptorTypeListCount > i) {
            const VkMutableDescriptorTypeListEXT& mutable_descriptor_type_list =
                    mutable_descriptor_type_create_info->pMutableDescriptorTypeLists[i];
            mutable_descriptor_types.insert(
                    mutable_descriptor_type_list.pDescriptorTypes,
                    mutable_descriptor_type_list.pDescriptorTypes +
                            mutable_descriptor_type_list.descriptorTypeCount);
        }

        const VkDescriptorPoolSize& pool_size = create_info.pPoolSizes[i];
        pool_sizes_.emplace(pool_size.type,
                            pool_size.descriptorCount / create_info.maxSets,
                            std::move(mutable_descriptor_types));
    }
}

void get_descriptor_set_layout_create_info_pnext_values(
        const void* pnext,
        const VkDescriptorPoolInlineUniformBlockCreateInfo*&
                descriptor_pool_inline_uniform_block_create_info,
        const VkMutableDescriptorTypeCreateInfoEXT*&
                mutable_descriptor_type_create_info) {
    if (!pnext) {
        return;
    }

    switch (*reinterpret_cast<const VkStructureType*>(pnext)) {
        case VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO:
            descriptor_pool_inline_uniform_block_create_info = reinterpret_cast<
                    const VkDescriptorPoolInlineUniformBlockCreateInfo*>(pnext);

            get_descriptor_set_layout_create_info_pnext_values(
                    descriptor_pool_inline_uniform_block_create_info->pNext,
                    descriptor_pool_inline_uniform_block_create_info,
                    mutable_descriptor_type_create_info);
            break;

        case VK_STRUCTURE_TYPE_MUTABLE_DESCRIPTOR_TYPE_CREATE_INFO_EXT:
            mutable_descriptor_type_create_info =
                    reinterpret_cast<const VkMutableDescriptorTypeCreateInfoEXT*>(
                            pnext);

            get_descriptor_set_layout_create_info_pnext_values(
                    mutable_descriptor_type_create_info->pNext,
                    descriptor_pool_inline_uniform_block_create_info,
                    mutable_descriptor_type_create_info);
            break;
    }
}

bool DescriptorSetManager::DescriptorPoolKey::operator==(
        const DescriptorPoolKey& rhs) const noexcept {
    return flags_ == rhs.flags_ && pool_sizes_ == rhs.pool_sizes_ &&
           max_inline_uniform_block_bindings_ == rhs.max_inline_uniform_block_bindings_;
}

DescriptorSetManager::DescriptorSetManager(VkDevice device)
        : descriptor_set_layout_manager_(device), descriptor_pool_managers_() {}

ManagedDescriptorSet DescriptorSetManager::allocate_descriptor_set(
        const std::vector<VkDescriptorSetLayoutBinding>&
                descriptor_set_layout_bindings) {
    return allocate_descriptor_set(
            get_descriptor_set_layout_create_info(descriptor_set_layout_bindings));
}

ManagedDescriptorSet DescriptorSetManager::allocate_descriptor_set(
        const VkDescriptorSetLayoutCreateInfo& descriptor_set_layout_create_info) {
    const DescriptorSetLayout& descriptor_set_layout =
            descriptor_set_layout_manager_.get_descriptor_set_layout(
                    descriptor_set_layout_create_info);

    std::vector<VkDescriptorType> descriptor_types =
            get_descriptor_types(descriptor_set_layout_create_info.pBindings,
                                 descriptor_set_layout_create_info.bindingCount);
    VkDescriptorPoolCreateInfo descriptor_pool_create_info =
            get_descriptor_pool_create_info(descriptor_types);
    DescriptorPoolKey key(descriptor_pool_create_info);
    auto it = descriptor_pool_managers_.find(key);
    if (it == descriptor_pool_managers_.end()) {
        auto it2 = descriptor_pool_managers_.insert(
                {std::move(key), DescriptorPoolManager(device(), descriptor_types)});
        it = it2.first;
    }

    DescriptorPoolSetAllocation descriptor_pool(it->second);
    return ManagedDescriptorSet(std::move(descriptor_pool), *descriptor_set_layout);
}
}  // namespace maseya::vkbase
