#pragma once

#include <vulkan/vulkan_core.h>

#include "UniqueObject.hxx"

namespace maseya::vkbase {
class Fence {
    struct Destroyer {
        constexpr Destroyer() noexcept : device(nullptr) {}

        Destroyer(VkDevice device) noexcept;

        void operator()(VkFence fence) const noexcept;

        VkDevice device;
    };

public:
    constexpr Fence(std::nullptr_t) noexcept : fence_(nullptr) {}

    Fence(VkDevice device, VkFenceCreateFlags flags = 0);

    VkFence operator*() const noexcept { return *fence_; }

private:
    UniqueObject<VkFence, Destroyer> fence_;
};
}  // namespace maseya::vkbase
