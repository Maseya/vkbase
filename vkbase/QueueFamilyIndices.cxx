#include "QueueFamilyIndices.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
QueueFamilyIndices::QueueFamilyIndices(VkPhysicalDevice physical_device,
                                       VkBool32 exclusive_transfer_queue)
        : graphics_family_(get_graphics_queue_family_index(physical_device)),
          transfer_family_(get_transfer_queue_family_index(physical_device,
                                                           exclusive_transfer_queue)) {}

std::unordered_set<uint32_t> QueueFamilyIndices::queue_family_indices() const {
    return {graphics_family(), transfer_family()};
}
}  // namespace maseya::vkbase