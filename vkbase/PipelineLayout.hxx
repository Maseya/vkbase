#pragma once

#include <vulkan/vulkan_core.h>

#include "UniqueObject.hxx"

namespace maseya::vkbase {
class PipelineLayout {
    struct Destroyer {
        constexpr Destroyer() noexcept : device(nullptr) {}

        Destroyer(VkDevice device);

        void operator()(VkPipelineLayout pipeline_layout) const noexcept;

        VkDevice device;
    };

public:
    constexpr PipelineLayout(std::nullptr_t) noexcept : pipeline_layout_(nullptr) {}

    PipelineLayout(VkDevice device, const VkDescriptorSetLayout* descriptor_set_layouts,
                   std::uint32_t count);
    PipelineLayout(VkDevice device, VkDescriptorSetLayout descriptor_set_layout);
    PipelineLayout(VkDevice device,
                   const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts);
    template <std::uint32_t N>
    PipelineLayout(VkDevice device,
                   const VkDescriptorSetLayout (&descriptor_set_layouts)[N])
            : PipelineLayout(device, descriptor_set_layouts, N) {}

    PipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo& create_info);

    PipelineLayout(const PipelineLayout&) = delete;
    PipelineLayout(PipelineLayout&&) noexcept = default;

    PipelineLayout& operator=(const PipelineLayout&) = delete;
    PipelineLayout& operator=(PipelineLayout&&) noexcept = default;

    VkPipelineLayout operator*() const noexcept { return *pipeline_layout_; }

    explicit operator bool() const noexcept {
        return static_cast<bool>(pipeline_layout_);
    }

private:
    UniqueObject<VkPipelineLayout, Destroyer> pipeline_layout_;
};
}  // namespace maseya::vkbase
