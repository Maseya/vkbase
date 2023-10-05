#include "vma_helper.hxx"

namespace maseya::vkbase {
VmaAllocator create_allocator(VkInstance instance, VkPhysicalDevice physical_device,
                              VkDevice device) {
    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.vulkanApiVersion = vulkan_api_version;
    allocator_info.instance = instance;
    allocator_info.physicalDevice = physical_device;
    allocator_info.device = device;
    allocator_info.pAllocationCallbacks = nullptr;

    VmaAllocator result;
    assert_result(vmaCreateAllocator(&allocator_info, &result));

    return result;
}

vma_buffer create_buffer(VmaAllocator allocator, VkDeviceSize size,
                         VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage,
                         VmaAllocationCreateFlags allocation_flags) {
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    // TODO(nrg): Clarify why this must always be true.
    // Should always be true. Use barriers to transition across queues.
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    buffer_info.size = size;
    buffer_info.usage = buffer_usage;

    VmaAllocationCreateInfo allocation_info{};
    allocation_info.flags = allocation_flags;
    allocation_info.usage = memory_usage;

    vma_buffer result{};
    assert_result(vmaCreateBuffer(allocator, &buffer_info, &allocation_info,
                                  &result.buffer, &result.allocation, nullptr));

    return result;
}

void destroy_buffer(VmaAllocator allocator, vma_buffer& buffer) {
    vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
}

vma_image create_image(VmaAllocator allocator, VkImageType image_type,
                       const VkExtent2D extent, VkFormat format) {
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = image_type;
    image_info.extent.width = extent.width;
    image_info.extent.height = extent.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                       VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (image_type == VK_IMAGE_TYPE_2D) {
        image_info.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    // TODO(nrg): Look into sharing mode.
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;

    VmaAllocationCreateInfo allocation_info{};
    allocation_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    vma_image result{};
    assert_result(vmaCreateImage(allocator, &image_info, &allocation_info,
                                 &result.image, &result.allocation, nullptr));

    return result;
}

vma_image create_image(VmaAllocator allocator, uint32_t extent, VkFormat format) {
    return create_image(allocator, VK_IMAGE_TYPE_1D, {extent, 1}, format);
}

vma_image create_image(VmaAllocator allocator, const VkExtent2D& extent,
                       VkFormat format) {
    return create_image(allocator, VK_IMAGE_TYPE_2D, extent, format);
}

void destroy_image(VmaAllocator allocator, vma_image& image) {
    vmaDestroyImage(allocator, image.image, image.allocation);
}

vma_mapping create_mapping(VmaAllocator allocator, VmaAllocation allocation) {
    vma_mapping result{};
    result.allocation = allocation;
    assert_result(vmaMapMemory(allocator, result.allocation, &result.data));
    return result;
}

void unmap_memory(VmaAllocator allocator, vma_mapping& mapping) {
    vmaUnmapMemory(allocator, mapping.allocation);
}
}  // namespace maseya::vkbase