#pragma once

#include <vulkan/vulkan_core.h>

#include "QueueFamilyIndices.hxx"

namespace maseya::vkbase {
class PresentationQueueFamilyIndices : public QueueFamilyIndices {
public:
    PresentationQueueFamilyIndices(VkPhysicalDevice physical_device,
                                   VkSurfaceKHR surface,
                                   VkBool32 exclusive_transfer_queue = VK_FALSE);

    virtual bool is_complete() const {
        return QueueFamilyIndices::is_complete() && presentation_family_.has_value();
    }

    uint32_t presentation_family() const { return presentation_family_.value(); }

    virtual std::unordered_set<uint32_t> queue_family_indices() const;

private:
    std::optional<uint32_t> presentation_family_;
};
}  // namespace maseya::vkbase
