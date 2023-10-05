#pragma once

#include <vulkan/vulkan_core.h>

#include "UniqueObject.hxx"

namespace maseya::vkbase {
class Surface {
    class Destroyer {
    public:
        constexpr Destroyer() noexcept : instance(nullptr) {}

        Destroyer(VkInstance instance) noexcept;

        void operator()(VkSurfaceKHR surface) const noexcept;

        VkInstance instance;
    };

public:
    constexpr Surface(std::nullptr_t) noexcept : surface_(nullptr) {}

    Surface(VkInstance instance, VkSurfaceKHR surface);

    Surface(const Surface&) = delete;
    Surface(Surface&&) noexcept = default;

    Surface& operator=(const Surface&) = delete;
    Surface& operator=(Surface&&) noexcept = default;

    VkSurfaceKHR operator*() const noexcept { return *surface_; }

    explicit operator bool() const noexcept { return static_cast<bool>(surface_); }

private:
    UniqueObject<VkSurfaceKHR, Destroyer> surface_;
};
}  // namespace maseya::vkbase