#include "shader_helper.hxx"

#include <filesystem>

namespace maseya::vkbase {
namespace fs = std::filesystem;

std::vector<uint32_t> compile_shader(const std::string& source, ShaderKind shader_kind,
                                     const std::string& file_name,
                                     const std::vector<Define>& defines) {
    return Compiler().compile_shader(source, file_name.empty() ? "shader" : file_name,
                                     shader_kind, defines);
}

void to_lower(std::string& str) {
    auto lower = [](char c) -> char { return static_cast<char>(std::tolower(c)); };

    std::transform(str.begin(), str.end(), str.begin(), lower);
}

std::string to_lower_copy(const std::string& str) {
    std::string result = str;
    to_lower(result);
    return result;
}

ShaderKind get_shader_kind_from_file_name(const std::string& path) {
    std::string ext = fs::path(path).extension().string();
    to_lower(ext);

    if (ext == ".vert" || ext == ".vs") {
        return ShaderKind::VertexShader;
    } else if (ext == ".frag" || ext == ".fs") {
        return ShaderKind::FragmentShader;
    } else {
        throw InvalidPathError("Could not infer shader kind from path name.", path);
    }
}

std::vector<uint32_t> compile_shader_from_file(const std::string& path,
                                               const std::vector<Define>& defines) {
    return compile_shader_from_file(path, get_shader_kind_from_file_name(path),
                                    defines);
}

std::vector<uint32_t> compile_shader_from_file(const std::string& path,
                                               ShaderKind shader_kind,
                                               const std::vector<Define>& defines) {
    return Compiler().compile_shader(path, shader_kind, defines);
}
}  // namespace maseya::vkbase