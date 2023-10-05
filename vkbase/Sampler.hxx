#pragma once

#include <vulkan/vulkan_core.h>

#include "UniqueObject.hxx"

namespace maseya::vkbase {
class Sampler {
    struct Destroyer {
        constexpr Destroyer() noexcept : device(nullptr) {}

        Destroyer(VkDevice device) noexcept;

        void operator()(VkSampler sampler) const noexcept;

        VkDevice device;
    };

public:
    constexpr Sampler(std::nullptr_t) noexcept : sampler_() {}

    Sampler(VkDevice device, VkFilter mag_filter, VkFilter min_filter,
            VkSamplerAddressMode mode_u, VkSamplerAddressMode mode_v,
            VkSamplerAddressMode mode_w);
    Sampler(VkDevice device, VkFilter mag_filter, VkFilter min_filter,
            VkSamplerAddressMode address_mode =
                    VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
    Sampler(VkDevice device, VkSamplerAddressMode address_mode);
    Sampler(VkDevice device);

    Sampler(const Sampler&) = delete;
    Sampler(Sampler&&) noexcept = default;

    Sampler& operator=(const Sampler&) = delete;
    Sampler& operator=(Sampler&&) noexcept = default;

    VkSampler operator*() const noexcept { return *sampler_; }

    explicit operator bool() const noexcept { return static_cast<bool>(sampler_); }

private:
    UniqueObject<VkSampler, Destroyer> sampler_;
};
}  // namespace maseya::vkbase
