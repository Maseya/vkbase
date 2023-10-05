#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

#include "Compiler.hxx"

namespace maseya::vkbase {
std::vector<uint32_t> compile_shader(const std::string& source, ShaderKind shader_kind,
                                     const std::string& file_name,
                                     const std::vector<Define>& defines = {});

ShaderKind get_shader_kind_from_file_name(const std::string& path);

std::vector<uint32_t> compile_shader_from_file(const std::string& path,
                                               const std::vector<Define>& defines = {});

std::vector<uint32_t> compile_shader_from_file(const std::string& path,
                                               ShaderKind shader_kind,
                                               const std::vector<Define>& defines = {});

inline VkShaderModule create_shader_module(VkDevice device, const std::string& source,
                                           ShaderKind shader_kind,
                                           const std::string& file_name,
                                           const std::vector<Define>& defines = {}) {
    return create_shader_module(
            device, compile_shader(source, shader_kind, file_name, defines));
}

inline VkShaderModule create_shader_module_from_file(
        VkDevice device, const std::string& path,
        const std::vector<Define>& defines = {}) {
    return create_shader_module(device, compile_shader_from_file(path, defines));
}

inline VkShaderModule create_shader_module_from_file(
        VkDevice device, const std::string& path, ShaderKind shader_kind,
        const std::vector<Define>& defines = {}) {
    return create_shader_module(device,
                                compile_shader_from_file(path, shader_kind, defines));
}
}  // namespace maseya::vkbase