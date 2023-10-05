#include "PersistantlyMappedBuffer.hxx"

#include "vma_helper.hxx"

namespace maseya::vkbase {
void PersistantlyMappedBuffer::Destroyer ::operator()(
        vma_mapping& mapping) const noexcept {
    unmap_memory(allocator, mapping);
}

PersistantlyMappedBuffer::PersistantlyMappedBuffer(VmaAllocator allocator,
                                                   VkBufferUsageFlags usage,
                                                   VkDeviceSize size)
        : Buffer(allocator, usage, size, VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
                 VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
                         VMA_ALLOCATION_CREATE_MAPPED_BIT),
          mapping_({}, allocator) {
    mapping_.reset(create_mapping(allocator, allocation()));
}

PersistantlyMappedBuffer::PersistantlyMappedBuffer(VmaAllocator allocator,
                                                   VkBufferUsageFlags usage,
                                                   const void* data, size_t size)
        : PersistantlyMappedBuffer(allocator, usage, size) {
    std::memcpy(this->data(), data, size);
}
}  // namespace maseya::vkbase