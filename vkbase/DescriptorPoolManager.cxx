#include "DescriptorPoolManager.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
DescriptorPoolManager::InternalState::InternalState(
        VkDevice device, const std::vector<VkDescriptorType>& descriptor_types)
        : device_(device),
          descriptor_types_(descriptor_types),
          descriptor_pools_(),
          pool_availability_(default_pool_size + 1),
          remaining_sizes_(),
          highest_availability_(0),
          total_remaining_(0) {}

std::uint32_t DescriptorPoolManager::InternalState::reserve_descriptor() {
    // Decide which descriptor pool we can allocate from.
    if (highest_availability_ == 0) {
        // Current descriptor pool vector size also acts as the index for the
        // to-be-created descriptor pool which will become available.
        pool_availability_[default_pool_size].insert(
                static_cast<std::uint32_t>(descriptor_pools_.size()));

        descriptor_pools_.emplace_back(device_, descriptor_types_, default_pool_size));

        remaining_sizes_.push_back(default_pool_size);
        highest_availability_ = default_pool_size;
        total_remaining_ += default_pool_size;
    }

    std::uint32_t pool_index = *pool_availability_[highest_availability_].begin();
    --remaining_sizes_[pool_index];
    pool_availability_[highest_availability_].erase(pool_index);
    pool_availability_[highest_availability_ - 1].insert(pool_index);
    if (pool_availability_[highest_availability_].empty()) {
        --highest_availability_;
    }

    --total_remaining_;
    return pool_index;
}

void DescriptorPoolManager::InternalState::release_descriptor(
        std::uint32_t pool_index) {
    pool_availability_[remaining_sizes_[pool_index] + 1].insert(pool_index);
    pool_availability_[remaining_sizes_[pool_index]].erase(pool_index);
    ++remaining_sizes_[pool_index];
    ++total_remaining_;

    // Automatically release descriptor pools once we start getting back a lot of sets.
    // Note that we can only release a pool that got ALL of its sets back.
    if (remaining_sizes_[pool_index] == default_pool_size &&
        total_remaining_ >= 2 * default_pool_size) {
        pool_availability_[default_pool_size].erase(pool_index);
        released_pools_.insert(pool_index);
        total_remaining_ -= default_pool_size;
        remaining_sizes_[pool_index] = 0;
        descriptor_pools_[pool_index] = nullptr;

        // TODO(nrg): I need to add some kind of defragmentation routine to the
        // descriptor_pools_ vector size from growing too large. Idea: When size of
        // release_pools_ reaches half the size of descriptor_pools_, I fill in all the
        // holes. This may be really hard to do as changing pool indexes would screw
        // things up. Perhaps, rather than managing indices, I manage the actual
        // descriptor pool handles in an unordered map. The handles will never change.
        // This is low priority as the growth of descriptor_pools_ should be very
        // minimal at my current scale of operation.
    }

    highest_availability_ =
            std::max(highest_availability_, remaining_sizes_[pool_index]);
}

DescriptorPoolManager::DescriptorPoolManager(
        VkDevice device, const std::vector<VkDescriptorType>& descriptor_types)
        : internal_state_(std::make_shared<InternalState>(device, descriptor_types)) {}
}  // namespace maseya::vkbase