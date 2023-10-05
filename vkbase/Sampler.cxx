#include "Sampler.hxx"

#include "vulkan_helper.hxx"

namespace maseya::vkbase {
Sampler::Destroyer::Destroyer(VkDevice device) noexcept : device(device) {}

void Sampler::Destroyer::operator()(VkSampler sampler) const noexcept {
    vkDestroySampler(device, sampler, nullptr);
}

Sampler::Sampler(VkDevice device, VkFilter mag_filter, VkFilter min_filter,
                 VkSamplerAddressMode mode_u, VkSamplerAddressMode mode_v,
                 VkSamplerAddressMode mode_w)
        : sampler_(create_sampler(device, mag_filter, min_filter, mode_u, mode_v,
                                  mode_w),
                   device) {}

Sampler::Sampler(VkDevice device, VkFilter mag_filter, VkFilter min_filter,
                 VkSamplerAddressMode address_mode)
        : sampler_(create_sampler(device, mag_filter, min_filter, address_mode),
                   device) {}

Sampler::Sampler(VkDevice device, VkSamplerAddressMode address_mode)
        : sampler_(create_sampler(device, address_mode), device) {}

Sampler::Sampler(VkDevice device) : sampler_(create_sampler(device), device) {}
}  // namespace maseya::vkbase