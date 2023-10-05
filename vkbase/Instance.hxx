#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>
#include <string>

#include "DebugUtilsMessenger.hxx"
#include "UniqueObject.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
class Instance {
    struct Destroyer {
        void operator()(VkInstance instance) const noexcept;
    };

public:
    constexpr Instance(std::nullptr_t) noexcept
            : instance_(nullptr), debug_utils_messenger_(nullptr) {}

    Instance(const char* application_name) : Instance(std::string(application_name)) {}

    template <class UserCallback>
    Instance(UserCallback& debug_callback)
            : Instance(default_application_name, debug_callback) {}
    template <class UserCallback>
    Instance(const std::string& application_name, UserCallback& debug_callback)
            : Instance(application_name, default_application_version, debug_callback) {}
    template <class UserCallback>
    Instance(const std::string& application_name, uint32_t version,
             UserCallback& debug_callback)
            : Instance(application_name, version,
                       create_debug_messenger_info(debug_callback)) {}

    Instance(const VkDebugUtilsMessengerCreateInfoEXT& debug_messenger_create_info)
            : Instance(default_application_name, debug_messenger_create_info) {}
    Instance(const std::string& application_name,
             const VkDebugUtilsMessengerCreateInfoEXT& debug_messenger_create_info)
            : Instance(application_name, default_application_version,
                       debug_messenger_create_info) {}
    Instance(const std::string& application_name, uint32_t application_version,
             const VkDebugUtilsMessengerCreateInfoEXT& debug_messenger_create_info)
            : Instance(application_name, application_version,
                       std::optional<VkDebugUtilsMessengerCreateInfoEXT>(
                               debug_messenger_create_info)) {}

    Instance(const std::string& application_name = default_application_name,
             uint32_t application_version = default_application_version,
             const std::optional<VkDebugUtilsMessengerCreateInfoEXT>&
                     debug_messenger_create_info = std::nullopt);

    Instance(const Instance&) = delete;
    Instance(Instance&&) noexcept = default;

    Instance& operator=(const Instance&) = delete;
    Instance& operator=(Instance&&) noexcept = default;

    VkInstance operator*() const noexcept { return *instance_; }

    explicit operator bool() const noexcept { return static_cast<bool>(instance_); }

private:
    UniqueObject<VkInstance, Destroyer> instance_;
    DebugUtilsMessenger debug_utils_messenger_;
};
}  // namespace maseya::vkbase