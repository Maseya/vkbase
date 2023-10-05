#pragma once

#include <vulkan/vulkan_core.h>

#include <cstddef>

#include "UniqueObject.hxx"

namespace maseya::vkbase {
class CommandPool {
    struct Destroyer {
        constexpr Destroyer() noexcept : device(nullptr) {}

        Destroyer(VkDevice device) noexcept;

        void operator()(VkCommandPool command_pool) const noexcept;

        VkDevice device;
    };

public:
    constexpr CommandPool(std::nullptr_t) noexcept : command_pool_(nullptr) {}

    CommandPool(VkDevice device, uint32_t queue_family_index);

    CommandPool(const CommandPool&) = delete;
    CommandPool(CommandPool&&) noexcept = default;

    CommandPool& operator=(const CommandPool&) = delete;
    CommandPool& operator=(CommandPool&&) noexcept = default;

    VkCommandPool operator*() const noexcept { return command_pool_.get(); }

    explicit operator bool() const noexcept { return static_cast<bool>(command_pool_); }

private:
    UniqueObject<VkCommandPool, Destroyer> command_pool_;
};
}  // namespace maseya::vkbase
