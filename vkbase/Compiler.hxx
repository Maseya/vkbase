#pragma once

#include <shaderc/shaderc.h>

#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

#include "UniqueObject.hxx"
#include "vulkan_helper.hxx"

namespace maseya::vkbase {
enum class ShaderSourceLanguage {
    Glsl = shaderc_source_language_glsl,
    Hlsl = shaderc_source_language_hlsl,
};

enum class ShaderOptimizationLevel {
    Zero = shaderc_optimization_level_zero,
    Size = shaderc_optimization_level_size,
    Performance = shaderc_optimization_level_performance,
};

enum class ShaderTargetEnv {
    Vulkan = shaderc_target_env_vulkan,
    OpenGl = shaderc_target_env_opengl,
    OpenGlCompat = shaderc_target_env_opengl_compat,
    Default = shaderc_target_env_default,
};

enum class ShaderEnvVersion {
    Vulkan_1_0 = shaderc_env_version_vulkan_1_0,
    Vulkan_1_1 = shaderc_env_version_vulkan_1_1,
    Vulkan_1_2 = shaderc_env_version_vulkan_1_2,
    Vulkan_1_3 = shaderc_env_version_vulkan_1_3,
    OpenGl_4_5 = shaderc_env_version_opengl_4_5,
};

enum class ShaderCompilationStatus {
    Success = shaderc_compilation_status_success,
    InvalidStage = shaderc_compilation_status_invalid_stage,
    CompilationError = shaderc_compilation_status_compilation_error,
    InternalError = shaderc_compilation_status_internal_error,
    NullResultObject = shaderc_compilation_status_null_result_object,
    InvalidAssembly = shaderc_compilation_status_invalid_assembly,
    ValidationError = shaderc_compilation_status_validation_error,
    TransformationError = shaderc_compilation_status_transformation_error,
    ConfigurationError = shaderc_compilation_status_configuration_error,
};

enum class ShaderKind {
    VertexShader = shaderc_vertex_shader,
    FragmentShader = shaderc_fragment_shader,
    ComputeShader = shaderc_compute_shader,
    GeometryShader = shaderc_geometry_shader,
    TessControlShader = shaderc_tess_control_shader,
    TessEvaluationShader = shaderc_tess_evaluation_shader,
};

enum class ShaderIncludeType {
    Relative = shaderc_include_type_relative,
    Standard = shaderc_include_type_standard,
};

struct Define {
    std::string name;
    std::string value;
};

class ShaderCCompileError : public VkBaseError {
public:
    ShaderCCompileError(ShaderCompilationStatus status);
    ShaderCCompileError(ShaderCompilationStatus status, const char* message);
    ShaderCCompileError(ShaderCompilationStatus status, const std::string& message);

    ShaderCompilationStatus status() const { return status_; }

private:
    ShaderCompilationStatus status_;
};

class IncludeResult {
public:
    IncludeResult(const std::string& source_name, const std::string&& content);

    IncludeResult(const std::string& source_name, const std::string& content);

    IncludeResult(const std::string& source_name);

    IncludeResult(const IncludeResult& rhs);
    IncludeResult(IncludeResult&&) noexcept = default;

    IncludeResult& operator=(const IncludeResult& rhs);
    IncludeResult& operator=(IncludeResult&&) noexcept = default;

    static IncludeResult make_error(const std::string& error);

    const std::string& source_name() const noexcept { return source_name_; }

    const std::string& content() const noexcept { return content_; }

    const shaderc_include_result& operator*() const noexcept { return include_result_; }

private:
    std::string source_name_;
    std::string content_;
    shaderc_include_result include_result_;
};

class IncludeResolver {
public:
    IncludeResolver();
    IncludeResolver(std::size_t max_include_depth);

private:
    IncludeResult* resolve_include(const std::string& requested_source,
                                   ShaderIncludeType shader_include_type,
                                   const std::string& requesting_source,
                                   size_t include_depth) const;

    void release_include_result(IncludeResult* include_result) const;

    static shaderc_include_result* include_resolve_fn(void* user_data,
                                                      const char* requested_source,
                                                      int type,
                                                      const char* requesting_source,
                                                      size_t include_depth);

    static void include_result_release_fn(void* user_data,
                                          shaderc_include_result* result);

private:
    std::size_t max_include_depth_;
    std::vector<std::string> standard_include_paths_;

    friend class CompileOptions;
};

class CompileOptions {
    struct Destroyer {
        void operator()(shaderc_compile_options_t compile_options) const noexcept;
    };

public:
    CompileOptions();

    CompileOptions(ShaderSourceLanguage source_langaue);
    CompileOptions(ShaderSourceLanguage source_langaue,
                   ShaderOptimizationLevel optimization_level);
    CompileOptions(ShaderSourceLanguage source_langaue,
                   ShaderOptimizationLevel optimization_level,
                   ShaderTargetEnv target_env, ShaderEnvVersion env_version);

    CompileOptions(const CompileOptions& rhs);
    CompileOptions(CompileOptions&&) noexcept = default;

    CompileOptions& operator=(const CompileOptions& rhs);
    CompileOptions& operator=(CompileOptions&&) noexcept = default;

    void set_source_language(ShaderSourceLanguage source_language) const;
    void set_optimization_level(ShaderOptimizationLevel optimization_level) const;
    void set_target_env(ShaderTargetEnv target_env, ShaderEnvVersion env_version) const;
    void add_macro_definition(const Define& define) const;

    template <class InputIt>
    void add_macro_definitions(InputIt first, InputIt last) const {
        for (InputIt it = first; it != last; ++it) {
            add_macro_definition(*it);
        }
    }

private:
    void assert_exists() const;

private:
    UniqueObject<shaderc_compile_options_t, Destroyer> compile_options_;
    IncludeResolver include_resolver_;

    friend class CompilationResult;
};

class Compiler {
    struct Destroyer {
        void operator()(shaderc_compiler_t compiler) const noexcept;
    };

public:
    Compiler();

    Compiler(const Compiler&) = delete;
    Compiler(Compiler&&) noexcept = default;

    Compiler& operator=(const Compiler&) = delete;
    Compiler& operator=(Compiler&&) noexcept = default;

    std::vector<uint32_t> compile_shader(const std::string& input_filename,
                                         ShaderKind shader_kind,
                                         const std::string& entrypoint_name = "main",
                                         const std::vector<Define>& defines = {}) const;

    std::vector<uint32_t> compile_shader(const std::string& input_filename,
                                         ShaderKind shader_kind,
                                         const std::vector<Define>& defines) const;

    std::vector<uint32_t> compile_shader(const std::string& source,
                                         const std::string& input_filename,
                                         ShaderKind shader_kind,
                                         const std::vector<Define>& defines) const;

    std::vector<uint32_t> compile_shader(const std::string& source,
                                         const std::string& input_filename,
                                         ShaderKind shader_kind,
                                         const std::string& entrypoint_name = "main",
                                         const std::vector<Define>& defines = {}) const;

private:
    UniqueObject<shaderc_compiler_t, Destroyer> compiler_;
    CompileOptions compile_options_;

    friend class CompilationResult;
};

class CompilationResult {
    struct Destroyer {
        void operator()(shaderc_compilation_result_t result) const noexcept;
    };

public:
    CompilationResult(const Compiler& compiler, const std::string& source,
                      const std::string& input_filename, ShaderKind shader_kind,
                      const std::string& entrypoint_name,
                      const CompileOptions& options);

    size_t num_warnings() const noexcept;
    size_t num_errors() const noexcept;

    ShaderCompilationStatus status() const;

    const char* error_message() const;

    std::vector<uint32_t> spirv_code() const;

private:
    UniqueObject<shaderc_compilation_result_t, Destroyer> compilation_result_;
};

constexpr const char* get_error_message(ShaderCompilationStatus status) noexcept {
    switch (status) {
        case ShaderCompilationStatus::Success:
            return "Success";
        case ShaderCompilationStatus::InvalidStage:
            return "Invalid stage";
        case ShaderCompilationStatus::CompilationError:
            return "Compilation error";
        case ShaderCompilationStatus::InternalError:
            return "Internal error";
        case ShaderCompilationStatus::NullResultObject:
            return "Null result object";
        case ShaderCompilationStatus::InvalidAssembly:
            return "Invalid assembly";
        case ShaderCompilationStatus::ValidationError:
            return "Validation error";
        case ShaderCompilationStatus::TransformationError:
            return "Transformation error";
        case ShaderCompilationStatus::ConfigurationError:
            return "Configuration error";
        default:
            return nullptr;
    }
}
}  // namespace maseya::vkbase