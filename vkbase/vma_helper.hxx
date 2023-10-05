#pragma once

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
struct vma_buffer {
    VmaAllocation allocation;
    VkBuffer buffer;

    constexpr operator bool() const noexcept { return allocation; }
};

struct vma_image {
    VmaAllocation allocation;
    VkImage image;

    constexpr operator bool() const noexcept { return allocation; }
};

struct vma_mapping {
    VmaAllocation allocation;
    void* data;

    constexpr operator bool() const noexcept { return allocation; }
};

VmaAllocator create_allocator(VkInstance instance, VkPhysicalDevice physical_device,
                              VkDevice device);

vma_buffer create_buffer(VmaAllocator allocator, VkDeviceSize size,
                         VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage,
                         VmaAllocationCreateFlags allocation_flags);

void destroy_buffer(VmaAllocator allocator, vma_buffer& buffer);

vma_image create_image(VmaAllocator allocator, uint32_t extent, VkFormat format);

vma_image create_image(VmaAllocator allocator, const VkExtent2D& extent,
                       VkFormat format);

void destroy_image(VmaAllocator allocator, vma_image& image);

vma_mapping create_mapping(VmaAllocator allocator, VmaAllocation allocation);

void unmap_memory(VmaAllocator allocator, vma_mapping& mapping);
}  // namespace maseya::vkbase
