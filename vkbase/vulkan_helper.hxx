#pragma once

#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "VulkanError.hxx"

#if defined(MASEYA_VKBASE_FORCE_VALIDATION_LAYERS_ENABLED)
#define MASEYA_VKBASE_VALIDATION_LAYERS_ENABLED
#elif defined(MASEYA_VKBASE_FORCE_VALIDATION_LAYERS_DISABLED)
#undef MASEYA_VKBASE_VALIDATION_LAYERS_ENABLED
#elif defined(DEBUG) || defined(_DEBUG)
#define MASEYA_VKBASE_VALIDATION_LAYERS_ENABLED
#endif

constexpr bool operator==(const VkExtent2D& lhs, const VkExtent2D& rhs) noexcept {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}
constexpr bool operator!=(const VkExtent2D& lhs, const VkExtent2D& rhs) noexcept {
    return !(lhs == rhs);
}

namespace maseya::vkbase {
#if defined(MASEYA_VKBASE_VALIDATION_LAYERS_ENABLED)
constexpr static bool validation_layers_enabled = true;
#else
constexpr static bool validation_layers_enabled = false;
#endif

constexpr static char default_application_name[] = "";
constexpr static uint32_t default_application_version = VK_MAKE_VERSION(1, 0, 0);

constexpr static char engine_name[] = "vksnes v1.0.0";
constexpr static uint32_t engine_version = VK_MAKE_VERSION(1, 0, 0);
constexpr static uint32_t vulkan_api_version = VK_API_VERSION_1_0;

constexpr static VkDebugUtilsMessageSeverityFlagsEXT default_debug_severity_flags =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
constexpr static VkDebugUtilsMessageTypeFlagsEXT default_debug_message_type_flags =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

// A special constant signifying that certain blocking calls should never time out.
constexpr static uint64_t no_timeout = UINT64_MAX;

// Double buffering should be the ideal mode. No tearing can happen, and no frames can
// be lost. We don't need an unlimited FPS either as most SNES games are locked at 50 or
// 60 FPS.
constexpr VkPresentModeKHR preferred_present_mode = VK_PRESENT_MODE_FIFO_KHR;

constexpr bool is_minimized(const VkExtent2D& extent) noexcept {
    return extent.width == 0 || extent.height == 0;
}

constexpr VkExtent2D clamp(const VkExtent2D& extent, const VkExtent2D& min_extent,
                           const VkExtent2D& max_extent) {
    return VkExtent2D{std::clamp(extent.width, min_extent.width, max_extent.width),
                      std::clamp(extent.height, min_extent.height, max_extent.height)};
}

template <class FunctionPointer>
FunctionPointer get_instance_proc_addr(VkInstance instance, const std::string& name) {
    PFN_vkVoidFunction result = vkGetInstanceProcAddr(instance, name.c_str());
    if (!result) {
        std::stringstream ss;
        ss << "Could not find function: " << name;
        throw VkBaseError(ss.str());
    }

    return reinterpret_cast<FunctionPointer>(result);
}

#define GET_INSTANCE_PROC_ADDR(instance__, name__) \
    get_instance_proc_addr<PFN_##name__>(instance__, #name__)

template <class UserCallback>
VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
               VkDebugUtilsMessageTypeFlagsEXT message_type,
               const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* userp) {
    UserCallback& callback = *static_cast<UserCallback*>(userp);
    return callback(message_severity, message_type, *callback_data);
}

template <class UserCallback>
VkDebugUtilsMessengerCreateInfoEXT create_debug_messenger_info(
        UserCallback& user_callback,
        VkDebugUtilsMessageSeverityFlagsEXT severity = default_debug_severity_flags,
        VkDebugUtilsMessageTypeFlagsEXT type =
                default_debug_message_type_flags) noexcept {
    VkDebugUtilsMessengerCreateInfoEXT result{};
    result.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    result.messageSeverity = severity;
    result.messageType = type;
    result.pfnUserCallback = debug_callback<UserCallback>;
    result.pUserData = &user_callback;

    return result;
}

std::vector<VkLayerProperties> get_instance_layer_properties();

std::vector<const char*> get_required_instance_layers();

template <class InputIt>
std::vector<const char*> get_unsupported_instance_layers(InputIt first, InputIt last) {
    std::unordered_set<std::string> names;
    for (const VkLayerProperties& properties : get_instance_layer_properties()) {
        names.insert(properties.layerName);
    }

    std::vector<const char*> result;
    for (auto it = first; it != last; ++it) {
        if (names.find(*it) == names.end()) {
            result.push_back(*it);
        }
    }

    return result;
}

inline std::vector<const char*> get_unsupported_instance_layers(
        const char* const* layers, size_t count) {
    return get_unsupported_instance_layers(layers, layers + count);
}

inline std::vector<const char*> get_unsupported_instance_layers(
        const std::vector<const char*>& layers) {
    return get_unsupported_instance_layers(layers.begin(), layers.end());
}

template <size_t N>
std::vector<const char*> get_unsupported_instance_layers(const char* (&layers)[N]) {
    return get_unsupported_instance_layers(std::begin(layers), std::end(layers));
}

void assert_instance_layers_supported(const std::vector<const char*>& layers);

std::vector<VkExtensionProperties> get_instance_extension_properties();

std::vector<const char*> get_required_instance_extensions();

template <class InputIt>
std::vector<const char*> get_unsupported_instance_extensions(InputIt first,
                                                             InputIt last) {
    std::unordered_set<std::string> names;
    for (const auto& properties : get_instance_extension_properties()) {
        names.insert(properties.extensionName);
    }

    std::vector<const char*> result;
    for (InputIt it = first; it != last; ++it) {
        if (names.find(*it) == names.end()) {
            result.push_back(*it);
        }
    }

    return result;
}

inline std::vector<const char*> get_unsupported_instance_extensions(
        const char* const* extensions, size_t count) {
    return get_unsupported_instance_extensions(extensions, extensions + count);
}

inline std::vector<const char*> get_unsupported_instance_extensions(
        const std::vector<const char*>& extensions) {
    return get_unsupported_instance_extensions(extensions.begin(), extensions.end());
}

template <size_t N>
std::vector<const char*> get_unsupported_instance_extensions(
        const char* (&extensions)[N]) {
    return get_unsupported_instance_extensions(std::begin(extensions),
                                               std::end(extensions));
}

void assert_instance_extensions_supported(const std::vector<const char*>& extensions);

VkInstance create_instance(
        const std::string& application_name = default_application_name,
        uint32_t application_version = default_application_version,
        const std::optional<VkDebugUtilsMessengerCreateInfoEXT>&
                debug_messenger_create_info = std::nullopt);

PFN_vkDestroyDebugUtilsMessengerEXT get_debug_utils_messenger_destructor(
        VkInstance instance);

VkDebugUtilsMessengerEXT create_debug_utils_messenger(
        VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT& create_info);

std::vector<VkPhysicalDevice> get_physical_devices(VkInstance instance);

VkPhysicalDeviceProperties get_physical_device_properties(
        VkPhysicalDevice physical_device);

std::vector<VkExtensionProperties> get_device_extension_properties(
        VkPhysicalDevice physical_device);

std::vector<const char*> get_required_device_extensions();

template <class InputIt>
std::vector<const char*> get_unsupported_device_extensions(
        VkPhysicalDevice physical_device, InputIt first, InputIt last) {
    std::unordered_set<std::string> names;
    for (const auto& properties : get_device_extension_properties(physical_device)) {
        names.insert(properties.extensionName);
    }

    std::vector<const char*> result;
    for (InputIt it = first; it != last; ++it) {
        if (names.find(*it) == names.end()) {
            result.push_back(*it);
        }
    }

    return result;
}

inline std::vector<const char*> get_unsupported_device_extensions(
        VkPhysicalDevice physical_device, const char* const* extensions, size_t count) {
    return get_unsupported_device_extensions(physical_device, extensions,
                                             extensions + count);
}

inline std::vector<const char*> get_unsupported_device_extensions(
        VkPhysicalDevice physical_device, const std::vector<const char*>& extensions) {
    return get_unsupported_device_extensions(physical_device, extensions.begin(),
                                             extensions.end());
}

template <size_t N>
std::vector<const char*> get_unsupported_device_extensions(
        VkPhysicalDevice physical_device, const char* (&extensions)[N]) {
    return get_unsupported_device_extensions(physical_device, std::begin(extensions),
                                             std::end(extensions));
}

inline std::vector<const char*> get_unsupported_device_extensions(
        VkPhysicalDevice physical_device) {
    return get_unsupported_device_extensions(physical_device,
                                             get_required_device_extensions());
}

void assert_device_extensions_supported(VkPhysicalDevice physical_device,
                                        const std::vector<const char*>& extensions);

std::vector<VkQueueFamilyProperties> get_queue_family_properties(
        VkPhysicalDevice physical_device);

std::optional<uint32_t> get_graphics_queue_family_index(
        VkPhysicalDevice physical_device);

std::optional<uint32_t> get_transfer_queue_family_index(
        VkPhysicalDevice physical_device, VkBool32 exclusive = VK_FALSE);

VkDevice create_device(VkPhysicalDevice physical_device,
                       const std::unordered_set<uint32_t>& queue_family_indices);

VkQueue get_queue(VkDevice device, uint32_t queue_family_index,
                  uint32_t queue_index = 0);

VkRenderPass create_flat_render_pass(VkDevice device, VkFormat format,
                                     VkImageLayout initial_layout,
                                     VkImageLayout final_layout);

inline VkRenderPass create_flat_render_pass(VkDevice device, VkFormat format,
                                            VkImageLayout final_layout) {
    return create_flat_render_pass(device, format, VK_IMAGE_LAYOUT_UNDEFINED,
                                   final_layout);
}

inline VkRenderPass create_flat_render_pass(VkDevice device, VkFormat format) {
    return create_flat_render_pass(device, format,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

VkRenderPass create_depth_render_pass(VkDevice device, VkFormat color_format,
                                      VkFormat depth_format,
                                      VkImageLayout initial_layout,
                                      VkImageLayout final_layout);

inline VkRenderPass create_depth_render_pass(VkDevice device, VkFormat color_format,
                                             VkFormat depth_format,
                                             VkImageLayout final_layout) {
    return create_depth_render_pass(device, color_format, depth_format,
                                    VK_IMAGE_LAYOUT_UNDEFINED, final_layout);
}

inline VkRenderPass create_depth_render_pass(VkDevice device, VkFormat color_format,
                                             VkFormat depth_format) {
    return create_depth_render_pass(device, color_format, depth_format,
                                    VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
}

template <class InputIt>
std::vector<VkDescriptorType> get_descriptor_types(
        InputIt first_descriptor_set_layout_binding,
        InputIt last_descriptor_set_layout_binding) {
    std::vector<VkDescriptorType> result;
    for (auto it = first_descriptor_set_layout_binding;
         it != last_descriptor_set_layout_binding; ++it) {
        const VkDescriptorSetLayoutBinding& binding = *it;
        result.push_back(binding.descriptorType);
    }

    return result;
}

std::vector<VkDescriptorType> get_descriptor_types(
        const std::vector<VkDescriptorSetLayoutBinding>&
                descriptor_set_layout_bindings) {
    return get_descriptor_types(descriptor_set_layout_bindings.begin(),
                                descriptor_set_layout_bindings.end());
}

template <std::uint32_t N>
std::vector<VkDescriptorType> get_descriptor_types(
        const VkDescriptorSetLayoutBinding(&descriptor_set_layout_bindings)) {
    return get_descriptor_types(std::begin(descriptor_set_layout_bindings),
                                std::end(descriptor_set_layout_bindings));
}

std::vector<VkDescriptorType> get_descriptor_types(
        const VkDescriptorSetLayoutBinding* descriptor_set_layout_bindings,
        std::uint32_t count) {
    return get_descriptor_types(descriptor_set_layout_bindings,
                                descriptor_set_layout_bindings + count);
}

VkDescriptorSetLayoutCreateInfo get_descriptor_set_layout_create_info(
        const VkDescriptorSetLayoutBinding* bindings, uint32_t count);

inline VkDescriptorSetLayoutCreateInfo get_descriptor_set_layout_create_info(
        const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
    return get_descriptor_set_layout_create_info(
            bindings.data(), static_cast<uint32_t>(bindings.size()));
}

template <size_t N>
VkDescriptorSetLayoutCreateInfo get_descriptor_set_layout_create_info(
        const VkDescriptorSetLayoutBinding (&bindings)[N], uint32_t count) {
    return get_descriptor_set_layout_create_info(bindings, static_cast<uint32_t>(N));
}

VkDescriptorSetLayout create_descriptor_set_layout(
        VkDevice device, const VkDescriptorSetLayoutCreateInfo& create_info);

inline VkDescriptorSetLayout create_descriptor_set_layout(
        VkDevice device, const VkDescriptorSetLayoutBinding* bindings, uint32_t count) {
    return create_descriptor_set_layout(
            device, get_descriptor_set_layout_create_info(bindings, count));
}

inline VkDescriptorSetLayout create_descriptor_set_layout(
        VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
    return create_descriptor_set_layout(device, bindings.data(),
                                        static_cast<uint32_t>(bindings.size()));
}
template <size_t N>
VkDescriptorSetLayout create_descriptor_set_layout(
        VkDevice device, const VkDescriptorSetLayoutBinding (&bindings)[N]) {
    return create_descriptor_set_layout(device, bindings, static_cast<uint32_t>(N));
}

VkPipelineLayoutCreateInfo get_pipeline_layout_create_info(
        const VkDescriptorSetLayout* descriptor_set_layouts, uint32_t count);

template <std::uint32_t N>
VkPipelineLayoutCreateInfo get_pipeline_layout_create_info(
        const VkDescriptorSetLayout (&descriptor_set_layouts)[N]) {
    return get_pipeline_layout_create_info(descriptor_set_layouts, N);
}

inline VkPipelineLayoutCreateInfo get_pipeline_layout_create_info(
        const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts) {
    return get_pipeline_layout_create_info(
            descriptor_set_layouts.data(),
            static_cast<std::uint32_t>(descriptor_set_layouts.size()));
}

inline VkPipelineLayoutCreateInfo get_pipeline_layout_create_info(
        const VkDescriptorSetLayout& descriptor_set_layout) {
    return get_pipeline_layout_create_info(&descriptor_set_layout, 1);
}

VkPipelineLayout create_pipeline_layout(VkDevice device,
                                        const VkPipelineLayoutCreateInfo& create_info);

inline VkPipelineLayout create_pipeline_layout(
        VkDevice device, const VkDescriptorSetLayout* descriptor_set_layouts,
        std::uint32_t count) {
    return create_pipeline_layout(
            device, get_pipeline_layout_create_info(descriptor_set_layouts, count));
}

inline VkPipelineLayout create_pipeline_layout(
        VkDevice device, VkDescriptorSetLayout descriptor_set_layout) {
    return create_pipeline_layout(device, &descriptor_set_layout, 1);
}

inline VkPipelineLayout create_pipeline_layout(
        VkDevice device,
        const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts) {
    return create_pipeline_layout(
            device, descriptor_set_layouts.data(),
            static_cast<std::uint32_t>(descriptor_set_layouts.size()));
}

template <std::uint32_t N>
VkPipelineLayout create_pipeline_layout(
        VkDevice device, const VkDescriptorSetLayout (&descriptor_set_layouts)[N]) {
    return create_pipeline_layout(device, descriptor_set_layouts, N);
}

VkShaderModule create_shader_module(VkDevice device, const uint32_t* code, size_t size);

template <class T>
VkShaderModule create_shader_module(VkDevice device, const std::vector<T>& code) {
    return create_shader_module(device, static_cast<const uint32_t*>(code.data()),
                                code.size() * sizeof(T));
}

VkPipeline create_graphics_pipeline(
        VkDevice device, VkRenderPass render_pass, VkPipelineLayout pipeline_layout,
        VkShaderModule vertex_shader, VkShaderModule fragment_shader,
        const VkVertexInputBindingDescription* vertex_binding_descriptions,
        uint32_t vertex_binding_description_count,
        const VkVertexInputAttributeDescription* vertex_attribute_descriptions,
        uint32_t vertex_attribute_description_count);

inline VkPipeline create_graphics_pipeline(VkDevice device, VkRenderPass render_pass,
                                           VkPipelineLayout pipeline_layout,
                                           VkShaderModule vertex_shader,
                                           VkShaderModule fragment_shader) {
    return create_graphics_pipeline(device, render_pass, pipeline_layout, vertex_shader,
                                    fragment_shader, nullptr, 0, nullptr, 0);
}

VkSemaphore create_semaphore(VkDevice device);

std::vector<VkSemaphore> create_semaphores(VkDevice device, size_t size);

VkFence create_fence(VkDevice device, VkFenceCreateFlags flags = 0);

std::vector<VkFence> create_fences(VkDevice device, size_t size,
                                   VkFenceCreateFlags flags = 0);

bool wait_for_fences(VkDevice device, const VkFence* fences, uint32_t count,
                     uint64_t timeout = no_timeout);

inline bool wait_for_fence(VkDevice device, VkFence fence,
                           uint64_t timeout = no_timeout) {
    return wait_for_fences(device, &fence, 1, timeout);
}

inline bool wait_for_fences(VkDevice device, const std::vector<VkFence>& fences,
                            uint64_t timeout = no_timeout) {
    return wait_for_fences(device, fences.data(), static_cast<uint32_t>(fences.size()),
                           timeout);
}

template <size_t N>
bool wait_for_fences(VkDevice device, const VkFence (&fences)[N],
                     uint64_t timeout = no_timeout) {
    return wait_for_fences(device, fences, N, timeout);
}

inline bool is_fence_idle(VkDevice device, VkFence fence) {
    return wait_for_fence(device, fence, 0);
}

void reset_fences(VkDevice device, const VkFence* fences, uint32_t count);

inline void reset_fence(VkDevice device, VkFence fence) {
    reset_fences(device, &fence, 1);
}

inline void reset_fences(VkDevice device, const std::vector<VkFence>& fences) {
    reset_fences(device, fences.data(), static_cast<uint32_t>(fences.size()));
}

template <size_t N>
void reset_fences(VkDevice device, const VkFence (&fences)[N]) {
    reset_fences(device, fences, N);
}

VkCommandPool create_command_pool(VkDevice device, uint32_t queue_family_index);

std::vector<VkCommandBuffer> allocate_command_buffers(VkDevice device,
                                                      VkCommandPool command_pool,
                                                      uint32_t count);

inline VkCommandBuffer allocate_command_buffer(VkDevice device,
                                               VkCommandPool command_pool) {
    return allocate_command_buffers(device, command_pool, 1).front();
}

void begin_command(VkCommandBuffer command_buffer, VkCommandBufferUsageFlags flags = 0);

void end_command(VkCommandBuffer command_buffer);

void transition_layout(VkCommandBuffer command_buffer, VkImage image,
                       VkImageLayout old_layout, VkImageLayout new_layout);

// Requirement: image layout must be VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL.
void copy_buffer_to_image(VkCommandBuffer command_buffer, VkBuffer buffer,
                          VkImage image, uint32_t width, uint32_t height);

// Requirements: Image layout must be VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL.
void copy_image_to_buffer(VkCommandBuffer command_buffer, VkImage image, uint32_t width,
                          uint32_t height, VkBuffer buffer);

VkImageView create_image_view(
        VkDevice device, VkImage image, VkFormat format,
        VkImageAspectFlags aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT,
        VkImageViewType image_view_type = VK_IMAGE_VIEW_TYPE_2D);

VkSampler create_sampler(VkDevice device, VkFilter mag_filter, VkFilter min_filter,
                         VkSamplerAddressMode mode_u, VkSamplerAddressMode mode_v,
                         VkSamplerAddressMode mode_w);

inline VkSampler create_sampler(
        VkDevice device, VkFilter mag_filter, VkFilter min_filter,
        VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER) {
    return create_sampler(device, mag_filter, min_filter, address_mode, address_mode,
                          address_mode);
}

inline VkSampler create_sampler(VkDevice device, VkSamplerAddressMode address_mode) {
    return create_sampler(device, VK_FILTER_NEAREST, VK_FILTER_NEAREST, address_mode);
}

inline VkSampler create_sampler(VkDevice device) {
    return create_sampler(device, VK_FILTER_NEAREST, VK_FILTER_NEAREST);
}

VkFramebuffer create_framebuffer(VkDevice device, VkImageView image_view,
                                 VkRenderPass render_pass, const VkExtent2D& extent);

template <class InputIt>
VkDescriptorPoolCreateInfo get_descriptor_pool_create_info(
        InputIt first_descriptor_type, InputIt last_descriptor_type,
        uint32_t max_sets = 1) {
    std::unordered_map<VkDescriptorType, uint32_t> counter;
    for (auto src = first_descriptor_type; src != last_descriptor_type; ++src) {
        auto dest = counter.find(*src);
        if (dest != counter.end()) {
            dest->second++;
        } else {
            counter.insert(std::make_pair(descriptor, 1));
        }
    }

    std::vector<VkDescriptorPoolSize> pool_sizes;
    for (const auto& pair : counter) {
        pool_sizes.push_back({pair.first, pair.second * max_sets});
    }

    VkDescriptorPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    create_info.pPoolSizes = pool_sizes.data();
    create_info.maxSets = max_sets;
    create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    return create_info;
}

inline VkDescriptorPoolCreateInfo get_descriptor_pool_create_info(
        const std::vector<VkDescriptorType>& descriptors, uint32_t max_sets = 1) {
    return get_descriptor_pool_create_info(descriptors.begin(), descriptors.end(),
                                           max_sets);
}

template <std::uint32_t N>
inline VkDescriptorPoolCreateInfo get_descriptor_pool_create_info(
        const VkDescriptorType (&descriptors)[N], uint32_t max_sets = 1) {
    return get_descriptor_pool_create_info(std::begin(descriptors),
                                           std::end(descriptors), max_sets);
}

inline VkDescriptorPoolCreateInfo get_descriptor_pool_create_info(
        const VkDescriptorType* descriptors, std::uint32_t descriptor_count,
        uint32_t max_sets = 1) {
    return get_descriptor_pool_create_info(descriptors, descriptors + descriptor_count,
                                           max_sets);
}

VkDescriptorPool create_descriptor_pool(VkDevice device,
                                        const VkDescriptorPoolCreateInfo& create_info);

inline VkDescriptorPool create_descriptor_pool(
        VkDevice device, const std::vector<VkDescriptorType>& descriptors,
        uint32_t max_sets = 1) {
    return create_descriptor_pool(
            device, get_descriptor_pool_create_info(descriptors, max_sets));
}

template <std::uint32_t N>
inline VkDescriptorPool create_descriptor_pool(VkDevice device,
                                               const VkDescriptorType (&descriptors)[N],
                                               uint32_t max_sets = 1) {
    return create_descriptor_pool(
            device, get_descriptor_pool_create_info(descriptors, max_sets));
}

inline VkDescriptorPool create_descriptor_pool(VkDevice device,
                                               const VkDescriptorType* descriptors,
                                               std::uint32_t descriptor_count,
                                               uint32_t max_sets = 1) {
    return create_descriptor_pool(
            device,
            get_descriptor_pool_create_info(descriptors, descriptor_count, max_sets));
}

inline VkDescriptorPool create_descriptor_pool(
        VkDevice device, const std::vector<VkDescriptorType>& descriptors,
        uint32_t max_sets = 1) {
    return create_descriptor_pool(
            device, get_descriptor_pool_create_info(descriptors, max_sets));
}

VkDescriptorSet create_descriptor_set(VkDevice device, VkDescriptorPool pool,
                                      VkDescriptorSetLayout layout);
}  // namespace maseya::vkbase