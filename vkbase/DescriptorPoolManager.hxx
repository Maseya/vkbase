#pragma once

#include <vulkan/vulkan_core.h>

#include <memory>
#include <unordered_set>
#include <vector>

#include "DescriptorPool.hxx"

namespace maseya::vkbase {
class DescriptorPoolManager {
    class InternalState {
        constexpr static std::uint32_t default_pool_size = 32;

    public:
        InternalState(VkDevice device,
                      const std::vector<VkDescriptorType>& descriptor_types);

        InternalState(const InternalState&) = delete;
        InternalState(InternalState&&) noexcept = default;

        InternalState& operator=(const InternalState&) = delete;
        InternalState& operator=(InternalState&&) noexcept = default;

        VkDevice device() const noexcept { return device_; }

        std::uint32_t reserve_descriptor();

        void release_descriptor(std::uint32_t pool_index);

        VkDescriptorPool operator[](std::uint32_t index) const noexcept {
            return *descriptor_pools_[index];
        }

    private:
        VkDevice device_;
        std::vector<VkDescriptorType> descriptor_types_;

        std::vector<DescriptorPool> descriptor_pools_;

        std::vector<std::unordered_set<std::uint32_t>> pool_availability_;

        std::vector<std::uint32_t> remaining_sizes_;

        std::uint32_t highest_availability_;

        std::uint32_t total_remaining_;

        std::unordered_set<std::uint32_t> released_pools_;
    };

public:
    DescriptorPoolManager(std::nullptr_t) noexcept : internal_state_(nullptr) {}

    DescriptorPoolManager(VkDevice device,
                          const std::vector<VkDescriptorType>& descriptor_types);

    DescriptorPoolManager(const DescriptorPoolManager&) = delete;
    DescriptorPoolManager(DescriptorPoolManager&&) noexcept = default;

    DescriptorPoolManager& operator=(const DescriptorPoolManager&) = delete;
    DescriptorPoolManager& operator=(DescriptorPoolManager&&) noexcept = default;

    VkDevice device() const noexcept { return internal_state_->device(); }

    explicit operator bool() const noexcept {
        return static_cast<bool>(internal_state_);
    }

private:
    std::shared_ptr<InternalState> internal_state_;

    friend class ManagedDescriptorSet;
    friend class DescriptorPoolSetAllocation;
};
}  // namespace maseya::vkbase
