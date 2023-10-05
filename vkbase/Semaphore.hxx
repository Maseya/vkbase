#pragma once

#include <vulkan/vulkan_core.h>

#include "UniqueObject.hxx"

namespace maseya::vkbase {
class Semaphore {
    struct Destroyer {
        constexpr Destroyer() noexcept : device(nullptr) {}

        Destroyer(VkDevice device) noexcept;

        void operator()(VkSemaphore semaphore) const noexcept;

        VkDevice device;
    };

public:
    constexpr Semaphore(std::nullptr_t) noexcept : semaphore_(nullptr) {}

    Semaphore(VkDevice device);

    VkSemaphore operator*() const noexcept { return *semaphore_; }

    explicit operator bool() const noexcept { return static_cast<bool>(semaphore_); }

private:
    UniqueObject<VkSemaphore, Destroyer> semaphore_;
};
}  // namespace maseya::vkbase
