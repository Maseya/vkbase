#include "DebugUtilsMessenger.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
DebugUtilsMessenger::Destroyer::Destroyer(VkInstance instance)
        : instance(instance),
          destroyer(get_debug_utils_messenger_destructor(instance)) {}

void DebugUtilsMessenger::Destroyer::operator()(
        VkDebugUtilsMessengerEXT messenger) const noexcept {
    if constexpr (validation_layers_enabled) {
        destroyer(instance, messenger, nullptr);
    }
}

DebugUtilsMessenger::DebugUtilsMessenger(
        VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT& create_info)
        : debug_utils_messenger_(VK_NULL_HANDLE, instance) {
    debug_utils_messenger_.reset(create_debug_utils_messenger(instance, create_info));
}
}  // namespace maseya::vkbase