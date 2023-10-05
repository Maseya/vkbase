#include "vulkan_helper.hxx"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>

#include "Compiler.hxx"
#include "VulkanError.hxx"

namespace maseya::vkbase {
namespace fs = std::filesystem;

std::vector<VkLayerProperties> get_instance_layer_properties() {
    uint32_t count;
    assert_result(vkEnumerateInstanceLayerProperties(&count, nullptr));

    std::vector<VkLayerProperties> result(count);
    assert_result(vkEnumerateInstanceLayerProperties(&count, result.data()));

    return result;
}

std::vector<const char*> get_required_instance_layers() {
    std::vector<const char*> result;
    if constexpr (validation_layers_enabled) {
        result.push_back("VK_LAYER_KHRONOS_validation");
    }

    return result;
}

void assert_instance_layers_supported(const std::vector<const char*>& instance_layers) {
    std::vector<const char*> unsupported_instance_layers =
            get_unsupported_instance_layers(instance_layers);

    if (!unsupported_instance_layers.empty()) {
        std::stringstream ss;
        ss << "The following Vulkan instance layers are required but are not supported "
              "on this system:";
        for (const std::string& name : unsupported_instance_layers) {
            ss << std::endl << name;
        }

        throw UnsupportedInstaceLayersError(ss.str(),
                                            std::move(unsupported_instance_layers));
    }
}

std::vector<VkExtensionProperties> get_instance_extension_properties() {
    uint32_t count;
    assert_result(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));

    std::vector<VkExtensionProperties> result(count);
    assert_result(
            vkEnumerateInstanceExtensionProperties(nullptr, &count, result.data()));

    return result;
}

std::vector<const char*> get_required_instance_extensions() {
    std::vector<const char*> result({
            VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_WIN32_KHR
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
    });

    if constexpr (validation_layers_enabled) {
        result.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return result;
}

void assert_instance_extensions_supported(const std::vector<const char*>& extensions) {
    std::vector<const char*> unsupported_instance_extensions =
            get_unsupported_instance_extensions(extensions);

    if (!unsupported_instance_extensions.empty()) {
        std::stringstream ss;
        ss << "The following Vulkan instance extensions are required but are not "
              "supported on this system:";
        for (const std::string& name : unsupported_instance_extensions) {
            ss << std::endl << name;
        }

        throw UnsupportedInstaceExtensionsError(
                ss.str(), std::move(unsupported_instance_extensions));
    }
}

VkInstance create_instance(const std::string& application_name,
                           uint32_t application_version,
                           const std::optional<VkDebugUtilsMessengerCreateInfoEXT>&
                                   debug_messenger_create_info) {
    std::vector<const char*> required_layers = get_required_instance_layers();
    assert_instance_layers_supported(required_layers);

    std::vector<const char*> required_extensions = get_required_instance_extensions();
    assert_instance_extensions_supported(required_extensions);

    // Next we fill out some app info for Vulkan's sake.
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = application_name.c_str();
    app_info.applicationVersion = application_version;
    app_info.pEngineName = engine_name;
    app_info.engineVersion = engine_version;
    app_info.apiVersion = vulkan_api_version;

    // Start filling out our instance info.
    VkInstanceCreateInfo instance_info{};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledExtensionCount =
            static_cast<uint32_t>(required_extensions.size());
    instance_info.ppEnabledExtensionNames = required_extensions.data();
    instance_info.enabledLayerCount = static_cast<uint32_t>(required_layers.size());
    instance_info.ppEnabledLayerNames = required_layers.data();

    // Enable validation layers for the instance if requested.
    if constexpr (validation_layers_enabled) {
        if (debug_messenger_create_info.has_value()) {
            instance_info.pNext = &debug_messenger_create_info.value();
        }
    }

    // Finally we create the instance.
    VkInstance result;
    assert_result(vkCreateInstance(&instance_info, nullptr, &result));

    return result;
}

PFN_vkDestroyDebugUtilsMessengerEXT get_debug_utils_messenger_destructor(
        VkInstance instance) {
    if constexpr (validation_layers_enabled) {
        return GET_INSTANCE_PROC_ADDR(instance, vkDestroyDebugUtilsMessengerEXT);
    } else {
        return nullptr;
    }
}

VkDebugUtilsMessengerEXT create_debug_utils_messenger(
        VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT& create_info) {
    if constexpr (validation_layers_enabled) {
        // Get function to create debug messenger.
        auto create_messenger_proc =
                GET_INSTANCE_PROC_ADDR(instance, vkCreateDebugUtilsMessengerEXT);

        VkDebugUtilsMessengerEXT result;
        assert_result(create_messenger_proc(instance, &create_info, nullptr, &result));
        return result;
    } else {
        return VK_NULL_HANDLE;
    }
}

std::vector<VkPhysicalDevice> get_physical_devices(VkInstance instance) {
    uint32_t count;
    assert_result(vkEnumeratePhysicalDevices(instance, &count, nullptr));

    std::vector<VkPhysicalDevice> result(count);
    assert_result(vkEnumeratePhysicalDevices(instance, &count, result.data()));

    return result;
}

VkPhysicalDeviceProperties get_physical_device_properties(
        VkPhysicalDevice physical_device) {
    VkPhysicalDeviceProperties result;
    vkGetPhysicalDeviceProperties(physical_device, &result);

    return result;
}

std::vector<VkExtensionProperties> get_device_extension_properties(
        VkPhysicalDevice physical_device) {
    uint32_t count;
    assert_result(vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count,
                                                       nullptr));

    std::vector<VkExtensionProperties> result(count);
    assert_result(vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count,
                                                       result.data()));

    return result;
}

std::vector<const char*> get_required_device_extensions() {
    return std::vector<const char*>({
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    });
}

void assert_device_extensions_supported(VkPhysicalDevice physical_device,
                                        const std::vector<const char*>& extensions) {
    std::vector<const char*> unsupported_device_extensions =
            get_unsupported_device_extensions(physical_device, extensions);

    if (!unsupported_device_extensions.empty()) {
        std::stringstream ss;
        ss << "The following Vulkan device extensions were requested but are not "
              "supported on this system:";
        for (const std::string& name : unsupported_device_extensions) {
            ss << std::endl << name;
        }

        throw UnsupportedDeviceExtensionsError(
                ss.str(), std::move(unsupported_device_extensions));
    }
}

std::vector<VkQueueFamilyProperties> get_queue_family_properties(
        VkPhysicalDevice physical_device) {
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, nullptr);

    std::vector<VkQueueFamilyProperties> result(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, result.data());

    return result;
}

std::optional<uint32_t> get_graphics_queue_family_index(
        VkPhysicalDevice physical_device) {
    std::vector<VkQueueFamilyProperties> families =
            get_queue_family_properties(physical_device);
    for (uint32_t i = 0; i < families.size(); i++) {
        if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return i;
        }
    }
    return std::nullopt;
}

std::optional<uint32_t> get_transfer_queue_family_index(
        VkPhysicalDevice physical_device, VkBool32 exclusive) {
    std::vector<VkQueueFamilyProperties> families =
            get_queue_family_properties(physical_device);
    for (uint32_t i = 0; i < families.size(); i++) {
        VkQueueFlags flags = families[i].queueFlags;
        if (flags & VK_QUEUE_TRANSFER_BIT) {
            if (exclusive && flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) {
                continue;
            }

            return i;
        }
    }
    return std::nullopt;
}

VkDevice create_device(VkPhysicalDevice physical_device,
                       const std::unordered_set<uint32_t>& queue_family_indices) {
    std::vector<const char*> required_layers = get_required_instance_layers();
    assert_instance_layers_supported(required_layers);

    std::vector<const char*> required_extensions = get_required_device_extensions();
    assert_device_extensions_supported(physical_device, required_extensions);

    // Although we have a graphics queue and presentation queue, it's possible that they
    // may be one in the same. Therefore, we create a set of unique queues and populate
    // them as such. Right now, no queue will have priority over another, so each will
    // just have the default max value.
    static constexpr float queue_priority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    for (uint32_t queue_family_index : queue_family_indices) {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family_index;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;

        queue_create_infos.push_back(queue_create_info);
    }

    // We aren't requesting any physical device features, but we still require a valid
    // instance of it to create a device.
    VkPhysicalDeviceFeatures enabled_features{};

    // Populate info for creating device.
    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.pEnabledFeatures = &enabled_features;
    create_info.enabledExtensionCount =
            static_cast<uint32_t>(required_extensions.size());
    create_info.ppEnabledExtensionNames = required_extensions.data();
    create_info.enabledLayerCount = static_cast<uint32_t>(required_layers.size());
    create_info.ppEnabledLayerNames = required_layers.data();

    VkDevice result;
    assert_result(vkCreateDevice(physical_device, &create_info, nullptr, &result));

    return result;
}

VkQueue get_queue(VkDevice device, uint32_t queue_family_index, uint32_t queue_index) {
    VkQueue result;
    vkGetDeviceQueue(device, queue_family_index, queue_index, &result);

    return result;
}

VkRenderPass create_flat_render_pass(VkDevice device, VkFormat format,
                                     VkImageLayout initial_layout,
                                     VkImageLayout final_layout) {
    VkAttachmentDescription color_attachment{};
    color_attachment.format = format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = initial_layout == VK_IMAGE_LAYOUT_UNDEFINED
                                      ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                      : VK_ATTACHMENT_LOAD_OP_LOAD;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = initial_layout;
    color_attachment.finalLayout = final_layout;

    VkAttachmentReference color_attachment_reference{};
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;

    VkRenderPassCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.attachmentCount = 1;
    create_info.pAttachments = &color_attachment;
    create_info.subpassCount = 1;
    create_info.pSubpasses = &subpass;

    VkRenderPass result;
    assert_result(vkCreateRenderPass(device, &create_info, nullptr, &result));

    return result;
}

VkRenderPass create_depth_render_pass(VkDevice device, VkFormat color_format,
                                      VkFormat depth_format,
                                      VkImageLayout initial_layout,
                                      VkImageLayout final_layout) {
    VkAttachmentDescription attachments[2]{};

    VkAttachmentDescription& color_attachment = attachments[0];
    color_attachment.format = color_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = initial_layout == VK_IMAGE_LAYOUT_UNDEFINED
                                      ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                      : VK_ATTACHMENT_LOAD_OP_LOAD;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = initial_layout;
    color_attachment.finalLayout = final_layout;

    VkAttachmentDescription& depth_attachment = attachments[1];
    depth_attachment.format = depth_format;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference color_attachment_reference{};
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkRenderPassCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.attachmentCount = 1;
    create_info.pAttachments = attachments;
    create_info.subpassCount = static_cast<uint32_t>(std::size(attachments));
    create_info.pSubpasses = &subpass;

    VkRenderPass result = VK_NULL_HANDLE;
    assert_result(vkCreateRenderPass(device, &create_info, nullptr, &result));
    return result;
}

VkDescriptorSetLayoutCreateInfo get_descriptor_set_layout_create_info(
        const VkDescriptorSetLayoutBinding* bindings, uint32_t count) {
    VkDescriptorSetLayoutCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    create_info.bindingCount = count;
    create_info.pBindings = bindings;

    return create_info;
}

VkDescriptorSetLayout create_descriptor_set_layout(
        VkDevice device, const VkDescriptorSetLayoutCreateInfo& create_info) {
    VkDescriptorSetLayout result;
    assert_result(vkCreateDescriptorSetLayout(device, &create_info, nullptr, &result));
    return result;
}

VkPipelineLayoutCreateInfo get_pipeline_layout_create_info(
        const VkDescriptorSetLayout* descriptor_set_layouts, uint32_t count) {
    VkPipelineLayoutCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    create_info.setLayoutCount = count;
    create_info.pSetLayouts = descriptor_set_layouts;
    return create_info;
}

VkPipelineLayout create_pipeline_layout(VkDevice device,
                                        const VkPipelineLayoutCreateInfo& create_info) {
    VkPipelineLayout result;
    assert_result(vkCreatePipelineLayout(device, &create_info, nullptr, &result));

    return result;
}

VkShaderModule create_shader_module(VkDevice device, const uint32_t* code,
                                    size_t size) {
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = size;
    create_info.pCode = code;

    VkShaderModule result;
    assert_result(vkCreateShaderModule(device, &create_info, nullptr, &result));

    return result;
}

VkPipeline create_graphics_pipeline(
        VkDevice device, VkRenderPass render_pass, VkPipelineLayout pipeline_layout,
        VkShaderModule vertex_shader, VkShaderModule fragment_shader,
        const VkVertexInputBindingDescription* vertex_binding_descriptions,
        uint32_t vertex_binding_description_count,
        const VkVertexInputAttributeDescription* vertex_attribute_descriptions,
        uint32_t vertex_attribute_description_count) {
    VkPipelineShaderStageCreateInfo shader_stages[2]{};

    shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stages[0].module = vertex_shader;
    shader_stages[0].pName = "main";

    shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stages[1].module = fragment_shader;
    shader_stages[1].pName = "main";

    VkPipelineVertexInputStateCreateInfo vertex_state{};
    vertex_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_state.pVertexBindingDescriptions = vertex_binding_descriptions;
    vertex_state.vertexBindingDescriptionCount = vertex_binding_description_count;
    vertex_state.pVertexAttributeDescriptions = vertex_attribute_descriptions;
    vertex_state.vertexAttributeDescriptionCount = vertex_attribute_description_count;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state{};
    input_assembly_state.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterization_state{};
    rasterization_state.sType =
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state.lineWidth = 1.0f;
    rasterization_state.cullMode = VK_CULL_MODE_NONE;
    rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;

    VkPipelineMultisampleStateCreateInfo multisampling_state{};
    multisampling_state.sType =
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling_state.minSampleShading = 1.0f;

    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blend_state{};
    color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state.attachmentCount = 1;
    color_blend_state.pAttachments = &color_blend_attachment;

    constexpr static VkDynamicState dynamic_states[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
    };
    VkPipelineDynamicStateCreateInfo dynamic_state{};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<uint32_t>(std::size(dynamic_states));
    dynamic_state.pDynamicStates = dynamic_states;

    VkGraphicsPipelineCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.stageCount = static_cast<uint32_t>(std::size(shader_stages));
    create_info.pStages = shader_stages;
    create_info.pVertexInputState = &vertex_state;
    create_info.pInputAssemblyState = &input_assembly_state;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = &rasterization_state;
    create_info.pMultisampleState = &multisampling_state;
    create_info.pColorBlendState = &color_blend_state;
    create_info.pDynamicState = &dynamic_state;
    create_info.layout = pipeline_layout;
    create_info.renderPass = render_pass;
    create_info.subpass = 0;
    create_info.basePipelineIndex = -1;

    VkPipeline result;
    assert_result(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &create_info,
                                            nullptr, &result));

    return result;
}

VkSemaphore create_semaphore(VkDevice device) {
    VkSemaphoreCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore semaphore;
    assert_result(vkCreateSemaphore(device, &create_info, nullptr, &semaphore));

    return semaphore;
}

std::vector<VkSemaphore> create_semaphores(VkDevice device, size_t size) {
    std::vector<VkSemaphore> result(size);
    for (VkSemaphore& semaphore : result) {
        semaphore = create_semaphore(device);
    }

    return result;
}

VkFence create_fence(VkDevice device, VkFenceCreateFlags flags) {
    VkFenceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    create_info.flags = flags;

    VkFence fence;
    assert_result(vkCreateFence(device, &create_info, nullptr, &fence));

    return fence;
}

std::vector<VkFence> create_fences(VkDevice device, size_t size,
                                   VkFenceCreateFlags flags) {
    std::vector<VkFence> result(size);
    for (VkFence& fence : result) {
        fence = create_fence(device, flags);
    }

    return result;
}

bool wait_for_fences(VkDevice device, const VkFence* fences, uint32_t count,
                     uint64_t timeout) {
    if (count == 0) {
        return true;
    }

    VkResult result = vkWaitForFences(device, count, fences, VK_TRUE, timeout);
    assert_result(result);
    return result == VK_SUCCESS;
}

void reset_fences(VkDevice device, const VkFence* fences, uint32_t count) {
    assert_result(vkResetFences(device, count, fences));
}

VkCommandPool create_command_pool(VkDevice device, uint32_t queue_family_index) {
    VkCommandPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    create_info.queueFamilyIndex = queue_family_index;

    VkCommandPool result;
    assert_result(vkCreateCommandPool(device, &create_info, nullptr, &result));

    return result;
}

std::vector<VkCommandBuffer> allocate_command_buffers(VkDevice device,
                                                      VkCommandPool command_pool,
                                                      uint32_t count) {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = count;

    std::vector<VkCommandBuffer> result(count);
    assert_result(vkAllocateCommandBuffers(device, &alloc_info, result.data()));

    return result;
}

void begin_command(VkCommandBuffer command_buffer, VkCommandBufferUsageFlags flags) {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = flags;

    assert_result(vkBeginCommandBuffer(command_buffer, &begin_info));
}

void end_command(VkCommandBuffer command_buffer) {
    assert_result(vkEndCommandBuffer(command_buffer));
}

void transition_layout(VkCommandBuffer command_buffer, VkImage image,
                       VkImageLayout old_layout, VkImageLayout new_layout) {
    if (new_layout == old_layout) {
        return;
    }

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;

    // TODO(nrg): I may need to support this in the future.
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    switch (old_layout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            barrier.srcAccessMask = 0;

            // TODO(nrg): This stage is deprecated.
            source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            source_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            source_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            break;

        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            barrier.srcAccessMask = 0;

            // TODO(nrg): This stage is deprecated.
            source_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            break;

        default:
            throw VkBaseError("Unsupported old layout");
    }

    switch (new_layout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            destination_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            break;

        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            barrier.dstAccessMask = VK_ACCESS_NONE_KHR;

            // TODO(nrg): This stage is deprecated.
            destination_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            break;

        default:
            throw VkBaseError("Unsupported new layout");
    }

    vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0, nullptr,
                         0, nullptr, 1, &barrier);
}

void copy_buffer_to_image(VkCommandBuffer command_buffer, VkBuffer buffer,
                          VkImage image, uint32_t width, uint32_t height) {
    VkBufferImageCopy region{};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(command_buffer, buffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void copy_image_to_buffer(VkCommandBuffer command_buffer, VkImage image, uint32_t width,
                          uint32_t height, VkBuffer buffer) {
    VkBufferImageCopy region{};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageExtent = {width, height, 1};

    vkCmdCopyImageToBuffer(command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           buffer, 1, &region);
}

VkImageView create_image_view(VkDevice device, VkImage image, VkFormat format,
                              VkImageAspectFlags aspect_mask,
                              VkImageViewType image_view_type) {
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.viewType = image_view_type;
    create_info.format = format;
    create_info.image = image;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = aspect_mask;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    VkImageView result;
    assert_result(vkCreateImageView(device, &create_info, nullptr, &result));

    return result;
}

VkSampler create_sampler(VkDevice device, VkFilter mag_filter, VkFilter min_filter,
                         VkSamplerAddressMode mode_u, VkSamplerAddressMode mode_v,
                         VkSamplerAddressMode mode_w) {
    VkSamplerCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    create_info.magFilter = mag_filter;
    create_info.minFilter = min_filter;
    create_info.addressModeU = mode_u;
    create_info.addressModeV = mode_v;
    create_info.addressModeW = mode_w;
    create_info.anisotropyEnable = VK_FALSE;
    create_info.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
    create_info.unnormalizedCoordinates = VK_FALSE;
    create_info.compareEnable = VK_FALSE;
    create_info.compareOp = VK_COMPARE_OP_ALWAYS;
    create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    create_info.mipLodBias = 0.0f;
    create_info.minLod = 0.0f;
    create_info.maxLod = 0.0f;

    VkSampler result;
    assert_result(vkCreateSampler(device, &create_info, nullptr, &result));

    return result;
}

VkFramebuffer create_framebuffer(VkDevice device, VkImageView image_view,
                                 VkRenderPass render_pass, const VkExtent2D& extent) {
    VkFramebufferCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = render_pass;
    create_info.attachmentCount = 1;
    create_info.pAttachments = &image_view;
    create_info.width = extent.width;
    create_info.height = extent.height;
    create_info.layers = 1;

    VkFramebuffer result;
    assert_result(vkCreateFramebuffer(device, &create_info, nullptr, &result));

    return result;
}

VkDescriptorPoolCreateInfo get_descriptor_pool_create_info(
        const std::vector<VkDescriptorType>& descriptors, uint32_t max_sets) {
    std::unordered_map<VkDescriptorType, uint32_t> counter;
    for (const auto& descriptor : descriptors) {
        auto it = counter.find(descriptor);
        if (it != counter.end()) {
            it->second++;
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

VkDescriptorPool create_descriptor_pool(
        VkDevice device, const std::vector<VkDescriptorType>& descriptors,
        uint32_t max_sets) {
    return create_descriptor_pool(
            device, get_descriptor_pool_create_info(descriptors, max_sets));
}

VkDescriptorPool create_descriptor_pool(VkDevice device,
                                        const VkDescriptorPoolCreateInfo& create_info) {
    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    assert_result(
            vkCreateDescriptorPool(device, &create_info, nullptr, &descriptor_pool));
    return descriptor_pool;
}

VkDescriptorSet create_descriptor_set(VkDevice device, VkDescriptorPool pool,
                                      VkDescriptorSetLayout layout) {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout;

    VkDescriptorSet descriptor_set;
    assert_result(vkAllocateDescriptorSets(device, &alloc_info, &descriptor_set));

    return descriptor_set;
}
}  // namespace maseya::vkbase