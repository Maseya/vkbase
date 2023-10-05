#include "Instance.hxx"

#include "UniqueObject.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
void Instance::Destroyer::operator()(VkInstance instance) const noexcept {
    vkDestroyInstance(instance, nullptr);
}

Instance::Instance(const std::string& application_name, uint32_t application_version,
                   const std::optional<VkDebugUtilsMessengerCreateInfoEXT>&
                           debug_messenger_create_info)
        : instance_(create_instance(application_name, application_version,
                                    debug_messenger_create_info)),
          debug_utils_messenger_(nullptr) {
    if constexpr (validation_layers_enabled) {
        if (debug_messenger_create_info.has_value()) {
            debug_utils_messenger_ = DebugUtilsMessenger(
                    *instance_, debug_messenger_create_info.value());
        }
    }
}
}  // namespace maseya::vkbase