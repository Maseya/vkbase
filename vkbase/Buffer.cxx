#include "Buffer.hxx"

#include <algorithm>
#include <cstring>

#include "UniqueObject.hxx"
#include "VulkanError.hxx"

namespace maseya::vkbase {
Buffer::Destroyer::Destroyer(VmaAllocator allocator) noexcept : allocator(allocator) {}

void Buffer::Destroyer::operator()(vma_buffer& buffer) const noexcept {
    destroy_buffer(allocator, buffer);
}

Buffer::Buffer(VmaAllocator allocator, VkBufferUsageFlags usage, VkDeviceSize size,
               VmaMemoryUsage memory_usage, VmaAllocationCreateFlags allocation_flags)
        : size_(size), usage_(usage), buffer_(vma_buffer{}, allocator) {
    buffer_.reset(
            create_buffer(allocator, size, usage, memory_usage, allocation_flags));
}
}  // namespace maseya::vkbase