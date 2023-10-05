#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>

#include "UniqueObject.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
class DebugUtilsMessenger {
    struct Destroyer {
        constexpr Destroyer() noexcept : instance(nullptr), destroyer(nullptr) {}

        Destroyer(VkInstance instance);

        void operator()(VkDebugUtilsMessengerEXT messenger) const noexcept;

        VkInstance instance;
        PFN_vkDestroyDebugUtilsMessengerEXT destroyer;
    };

public:
    constexpr DebugUtilsMessenger(std::nullptr_t) noexcept
            : debug_utils_messenger_(VK_NULL_HANDLE) {}

    template <class UserCallback>
    DebugUtilsMessenger(VkInstance instance, UserCallback user_callback)
            : DebugUtilsMessenger(instance,
                                  create_debug_messenger_info(user_callback)) {}

    DebugUtilsMessenger(VkInstance instance,
                        const VkDebugUtilsMessengerCreateInfoEXT& create_info);

    DebugUtilsMessenger(const DebugUtilsMessenger&) = delete;
    DebugUtilsMessenger(DebugUtilsMessenger&&) noexcept = default;

    DebugUtilsMessenger& operator=(const DebugUtilsMessenger&) = delete;
    DebugUtilsMessenger& operator=(DebugUtilsMessenger&&) noexcept = default;

    VkDebugUtilsMessengerEXT operator*() const noexcept {
        return *debug_utils_messenger_;
    }

    explicit operator bool() const noexcept {
        return static_cast<bool>(debug_utils_messenger_);
    }

private:
    UniqueObject<VkDebugUtilsMessengerEXT, Destroyer> debug_utils_messenger_;
};
}  // namespace maseya::vkbase
