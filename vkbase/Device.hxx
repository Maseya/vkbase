#pragma once

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <unordered_set>

#include "UniqueObject.hxx"

namespace maseya::vkbase {
class Device {
    struct DeviceDestroyer {
        void operator()(VkDevice device) const noexcept;
    };

    struct AllocatorDestroyer {
        void operator()(VmaAllocator allocator) const noexcept;
    };

public:
    constexpr Device(std::nullptr_t) noexcept
            : device_(nullptr),
              allocator_(nullptr),
              default_image_format_(VK_FORMAT_UNDEFINED) {}

    Device(VkInstance instance, VkPhysicalDevice physical_device,
           const std::unordered_set<uint32_t>& queue_family_indices,
           VkFormat default_image_format);

    Device(const Device&) = delete;
    Device(Device&&) noexcept = default;

    ~Device();

    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) noexcept = default;

    VkDevice operator*() const noexcept { return device_.get(); }
    VmaAllocator allocator() const noexcept { return allocator_.get(); }

    VkFormat default_image_format() const noexcept { return default_image_format_; }

    explicit operator bool() const noexcept { return static_cast<bool>(device_); }

    void wait_idle() const;

private:
    UniqueObject<VkDevice, DeviceDestroyer> device_;
    UniqueObject<VmaAllocator, AllocatorDestroyer> allocator_;

    VkFormat default_image_format_;
};
}  // namespace maseya::vkbase