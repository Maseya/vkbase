#pragma once

#include "Buffer.hxx"
#include "vma_helper.hxx"

namespace maseya::vkbase {
class PersistantlyMappedBuffer : public Buffer {
    struct Destroyer {
        constexpr Destroyer() noexcept : allocator(nullptr) {}

        Destroyer(VmaAllocator allocator) noexcept;

        void operator()(vma_mapping& mapping) const noexcept;

        VmaAllocator allocator;
    };

public:
    constexpr PersistantlyMappedBuffer(std::nullptr_t) noexcept
            : Buffer(nullptr), mapping_(nullptr) {}

    PersistantlyMappedBuffer(VmaAllocator allocator, VkBufferUsageFlags usage,
                             VkDeviceSize size);
    PersistantlyMappedBuffer(VmaAllocator allocator, VkBufferUsageFlags usage,
                             const void* data, size_t size);
    template <class T>
    PersistantlyMappedBuffer(VmaAllocator allocator, VkBufferUsageFlags usage,
                             const std::vector<T>& data)
            : PersistantlyMappedBuffer(allocator, usage, data.data(),
                                       data.size() * sizeof(T)) {}
    template <class T, size_t N>
    PersistantlyMappedBuffer(VmaAllocator allocator, VkBufferUsageFlags usage,
                             const T (&data)[N])
            : PersistantlyMappedBuffer(allocator, usage, data, N * sizeof(T)) {}

    PersistantlyMappedBuffer(const PersistantlyMappedBuffer&) = delete;
    PersistantlyMappedBuffer(PersistantlyMappedBuffer&&) noexcept = default;

    PersistantlyMappedBuffer& operator=(const PersistantlyMappedBuffer&) = delete;
    PersistantlyMappedBuffer& operator=(PersistantlyMappedBuffer&&) noexcept = default;

    void* data() noexcept { return mapping_->data; }
    const void* data() const noexcept { return mapping_->data; }

private:
    UniqueObject<vma_mapping, Destroyer> mapping_;
};
}  // namespace maseya::vkbase
