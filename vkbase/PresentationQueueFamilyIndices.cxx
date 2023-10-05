#include "PresentationQueueFamilyIndices.hxx"

#include "vulkan_surface_helper.hxx"

namespace maseya::vkbase {
PresentationQueueFamilyIndices::PresentationQueueFamilyIndices(
        VkPhysicalDevice physical_device, VkSurfaceKHR surface,
        VkBool32 exclusive_transfer_queue)
        : QueueFamilyIndices(physical_device, exclusive_transfer_queue),
          presentation_family_(
                  get_presentation_queue_family_index(physical_device, surface)) {}

std::unordered_set<uint32_t> PresentationQueueFamilyIndices::queue_family_indices()
        const {
    std::unordered_set<uint32_t> result = QueueFamilyIndices::queue_family_indices();
    result.insert(transfer_family());
    return result;
}
}  // namespace maseya::vkbase