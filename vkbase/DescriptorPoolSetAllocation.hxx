#pragma once

#include <vulkan/vulkan_core.h>

#include <memory>

#include "DescriptorPoolManager.hxx"
#include "UniqueObject.hxx"

namespace maseya::vkbase {
class DescriptorPoolSetAllocation {
    struct Releaser {
        constexpr Releaser() : descriptor_pool_manager_internal_state(nullptr) {}

        Releaser(std::shared_ptr<DescriptorPoolManager::InternalState>&
                         descriptor_pool_manager_internal_state);

        void operator()(size_t pool_index) noexcept;

        std::shared_ptr<DescriptorPoolManager::InternalState>
                descriptor_pool_manager_internal_state;
    };

    DescriptorPoolSetAllocation(std::shared_ptr<DescriptorPoolManager::InternalState>&
                                        descriptor_pool_manager_internal_state);

public:
    DescriptorPoolSetAllocation(std::nullptr_t) noexcept
            : descriptor_pool_manager_internal_state_(nullptr),
              pool_index_(),
              descriptor_pool_{} {}

    DescriptorPoolSetAllocation(DescriptorPoolManager& descriptor_pool_manager);

    DescriptorPoolSetAllocation(const DescriptorPoolSetAllocation&) = delete;
    DescriptorPoolSetAllocation(DescriptorPoolSetAllocation&&) noexcept = default;

    DescriptorPoolSetAllocation& operator=(const DescriptorPoolSetAllocation&) = delete;
    DescriptorPoolSetAllocation& operator=(DescriptorPoolSetAllocation&&) noexcept =
            default;

    VkDevice device() const noexcept {
        return descriptor_pool_manager_internal_state_->device();
    }

    VkDescriptorPool descriptor_pool() const noexcept { descriptor_pool_; }

    explicit operator bool() const noexcept { return static_cast<bool>(pool_index_); }

private:
    std::shared_ptr<DescriptorPoolManager::InternalState>
            descriptor_pool_manager_internal_state_;
    UniqueObject<size_t, Releaser> pool_index_;
    VkDescriptorPool descriptor_pool_;
};
}  // namespace maseya::vkbase