#include "DescriptorPoolSetAllocation.hxx"

namespace maseya::vkbase {
DescriptorPoolSetAllocation::Releaser::Releaser(
        std::shared_ptr<DescriptorPoolManager::InternalState>&
                descriptor_pool_manager_internal_state)
        : descriptor_pool_manager_internal_state(
                  descriptor_pool_manager_internal_state) {}

void DescriptorPoolSetAllocation::Releaser::operator()(size_t pool_index) noexcept {
    // The DescriptorManager::InternalState destructor will set its device handle to
    // null, so we check for that before destroying. If the manager is destructed, then
    // this descriptor set is automatically released, so we don't need to do anything.
    if (descriptor_pool_manager_internal_state) {
        descriptor_pool_manager_internal_state->release_descriptor(pool_index);
    }
}

DescriptorPoolSetAllocation::DescriptorPoolSetAllocation(
        std::shared_ptr<DescriptorPoolManager::InternalState>&
                descriptor_pool_manager_internal_state)
        : descriptor_pool_manager_internal_state_(
                  descriptor_pool_manager_internal_state),
          pool_index_(0, descriptor_pool_manager_internal_state),
          descriptor_pool_(VK_NULL_HANDLE) {
    pool_index_.reset(descriptor_pool_manager_internal_state->reserve_descriptor());
    descriptor_pool_ = (*descriptor_pool_manager_internal_state)[*pool_index_];
}

DescriptorPoolSetAllocation::DescriptorPoolSetAllocation(
        DescriptorPoolManager& descriptor_pool_manager)
        : DescriptorPoolSetAllocation(descriptor_pool_manager.internal_state_) {}
}  // namespace maseya::vkbase
