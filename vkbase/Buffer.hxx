#pragma once

#include <vulkan/vulkan_core.h>

#include <cstddef>

#include "UniqueObject.hxx"
#include "vma_helper.hxx"

namespace maseya::vkbase {
class Buffer {
    struct Destroyer {
        constexpr Destroyer() noexcept : allocator(nullptr) {}

        Destroyer(VmaAllocator allocator) noexcept;

        void operator()(vma_buffer& buffer) const noexcept;

        VmaAllocator allocator;
    };

public:
    constexpr Buffer(std::nullptr_t) noexcept : size_{}, usage_{}, buffer_(nullptr) {}

    Buffer(VmaAllocator allocator, VkBufferUsageFlags usage, VkDeviceSize size,
           VmaMemoryUsage memory_usage, VmaAllocationCreateFlags allocation_flags);

    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) noexcept = default;

    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) noexcept = default;

    VkDeviceSize size() const noexcept { return size_; }
    VkBufferUsageFlags usage() const noexcept { return usage_; }

    VmaAllocation allocation() const noexcept { return buffer_->allocation; }
    VkBuffer operator*() const noexcept { return buffer_->buffer; }

    explicit operator bool() const noexcept { return static_cast<bool>(buffer_); }

private:
    VkDeviceSize size_;
    VkBufferUsageFlags usage_;
    UniqueObject<vma_buffer, Destroyer> buffer_;
};
}  // namespace maseya::vkbase
