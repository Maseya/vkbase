#pragma once

#include <vulkan/vulkan.h>

#include <optional>
#include <unordered_set>

namespace maseya::vkbase {
// Indices of physical device queue families that support desired operations.
class QueueFamilyIndices {
public:
    QueueFamilyIndices(VkPhysicalDevice physical_device,
                       VkBool32 exclusive_transfer_queue = VK_FALSE);

    virtual bool is_complete() const {
        return graphics_family_.has_value() && transfer_family_.has_value();
    }

    uint32_t graphics_family() const { return graphics_family_.value(); }
    uint32_t transfer_family() const { return transfer_family_.value(); }

    virtual std::unordered_set<uint32_t> queue_family_indices() const;

private:
    std::optional<uint32_t> graphics_family_;
    std::optional<uint32_t> transfer_family_;
};
}  // namespace maseya::vkbase