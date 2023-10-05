#include "Compiler.hxx"

#include <algorithm>
#include <filesystem>
#include <sstream>

#include "VulkanError.hxx"
#include "read_file.hxx"

namespace maseya::vkbase {
namespace fs = std::filesystem;

ShaderCCompileError::ShaderCCompileError(ShaderCompilationStatus status)
        : ShaderCCompileError(status, get_error_message(status)) {}
ShaderCCompileError::ShaderCCompileError(ShaderCompilationStatus status,
                                         const char* message)
        : VkBaseError(message), status_(status) {}
ShaderCCompileError::ShaderCCompileError(ShaderCompilationStatus status,
                                         const std::string& message)
        : ShaderCCompileError(status, message.c_str()) {}

IncludeResult::IncludeResult(const std::string& source_name,
                             const std::string&& content)
        : source_name_(source_name), content_(std::move(content)), include_result_{} {
    include_result_.source_name = source_name.empty() ? nullptr : source_name.c_str();
    include_result_.source_name_length = source_name.size();
    include_result_.content = content.empty() ? nullptr : content.c_str();
    include_result_.content_length = content.size();
    include_result_.user_data = reinterpret_cast<void*>(this);
}

IncludeResult::IncludeResult(const std::string& source_name, const std::string& content)
        : IncludeResult(source_name, std::string(content)) {}

IncludeResult::IncludeResult(const std::string& source_name)
        : IncludeResult(source_name, read_file(source_name)) {}

IncludeResult::IncludeResult(const IncludeResult& rhs)
        : IncludeResult(rhs.source_name_, rhs.content_) {}

IncludeResult& IncludeResult::operator=(const IncludeResult& rhs) {
    return *this = IncludeResult(rhs.source_name_, rhs.content_);
}

IncludeResult IncludeResult::make_error(const std::string& error) {
    return IncludeResult(std::string(), error);
}

IncludeResolver::IncludeResolver() : IncludeResolver(1000) {}

IncludeResolver::IncludeResolver(std::size_t max_include_depth)
        : max_include_depth_(max_include_depth), standard_include_paths_() {}

IncludeResult* IncludeResolver::resolve_include(const std::string& requested_source,
                                                ShaderIncludeType shader_include_type,
                                                const std::string& requesting_source,
                                                size_t include_depth) const {
    if (max_include_depth_ != 0 && include_depth > max_include_depth_) {
        std::stringstream ss;
        ss << "Exceeded max included depth (" << max_include_depth_ << ").";
        return new IncludeResult(IncludeResult::make_error(ss.str()));
    }

    fs::path source_path;
    if (shader_include_type == ShaderIncludeType::Standard) {
        auto source_path_it = std::find_if(
                standard_include_paths_.begin(), standard_include_paths_.end(),
                [&source_path, &requested_source](const std::string& dir) {
                    source_path = fs::path(dir) / requested_source;
                    return fs::exists(source_path);
                });
        if (source_path_it == standard_include_paths_.end()) {
            std::stringstream ss;
            ss << "Could not find include file: <" << requesting_source << ">.";
            return new IncludeResult(IncludeResult::make_error(ss.str()));
        }
    } else {
        fs::path input_path(requesting_source);
        source_path = input_path.parent_path() / requested_source;
        if (!fs::exists(source_path)) {
            std::stringstream ss;
            ss << "Could not find include file: \""
               << fs::absolute(source_path).string() << "\".";
            return new IncludeResult(IncludeResult::make_error(ss.str()));
        }
    }

    return new IncludeResult(source_path.string());
}

void IncludeResolver::release_include_result(IncludeResult* include_result) const {
    delete include_result;
}

shaderc_include_result* IncludeResolver::include_resolve_fn(
        void* user_data, const char* requested_source, int type,
        const char* requesting_source, size_t include_depth) {
    IncludeResolver& include_resolver = *reinterpret_cast<IncludeResolver*>(user_data);
    IncludeResult* result = include_resolver.resolve_include(
            requested_source, static_cast<ShaderIncludeType>(type), requesting_source,
            include_depth);
    return const_cast<shaderc_include_result*>(&result->operator*());
}

void IncludeResolver::include_result_release_fn(void* user_data,
                                                shaderc_include_result* result) {
    IncludeResolver& include_resolver = *reinterpret_cast<IncludeResolver*>(user_data);
    IncludeResult* include_result = reinterpret_cast<IncludeResult*>(result->user_data);
    include_resolver.release_include_result(include_result);
}

void CompileOptions::Destroyer::operator()(
        shaderc_compile_options_t compile_options) const noexcept {
    shaderc_compile_options_release(compile_options);
}

CompileOptions::CompileOptions()
        : compile_options_(shaderc_compile_options_initialize()),
          include_resolver_(1000) {
    assert_exists();
    shaderc_compile_options_set_include_callbacks(
            *compile_options_, IncludeResolver::include_resolve_fn,
            IncludeResolver::include_result_release_fn, &include_resolver_);
}

CompileOptions::CompileOptions(ShaderSourceLanguage source_langaue) : CompileOptions() {
    set_source_language(source_langaue);
}

CompileOptions::CompileOptions(ShaderSourceLanguage source_langaue,
                               ShaderOptimizationLevel optimization_level)
        : CompileOptions(source_langaue) {
    set_optimization_level(optimization_level);
}

CompileOptions::CompileOptions(ShaderSourceLanguage source_langaue,
                               ShaderOptimizationLevel optimization_level,
                               ShaderTargetEnv target_env, ShaderEnvVersion env_version)
        : CompileOptions(source_langaue, optimization_level) {
    set_target_env(target_env, env_version);
}

CompileOptions::CompileOptions(const CompileOptions& rhs)
        : compile_options_(shaderc_compile_options_clone(*rhs.compile_options_)) {
    assert_exists();

    // TODO(nrg): Do I need to add include function resolver here?
}

CompileOptions& CompileOptions::operator=(const CompileOptions& rhs) {
    CompileOptions result(rhs);
    return *this = std::move(result);
}

void CompileOptions::set_source_language(ShaderSourceLanguage source_language) const {
    shaderc_compile_options_set_source_language(
            *compile_options_, static_cast<shaderc_source_language>(source_language));
}

void CompileOptions::set_optimization_level(
        ShaderOptimizationLevel optimization_level) const {
    shaderc_compile_options_set_optimization_level(
            *compile_options_,
            static_cast<shaderc_optimization_level>(optimization_level));
}

void CompileOptions::set_target_env(ShaderTargetEnv target_env,
                                    ShaderEnvVersion env_version) const {
    shaderc_compile_options_set_target_env(
            *compile_options_, static_cast<shaderc_target_env>(target_env),
            static_cast<shaderc_env_version>(env_version));
}

void CompileOptions::add_macro_definition(const Define& define) const {
    shaderc_compile_options_add_macro_definition(
            *compile_options_, define.name.empty() ? nullptr : define.name.c_str(),
            define.name.size(), define.value.empty() ? nullptr : define.value.c_str(),
            define.value.size());
}

void CompileOptions::assert_exists() const {
    if (!compile_options_) {
        throw VkBaseError(
                "Could not initialize shaderc compile options. An unknown error "
                "occurred.");
    }
}

void Compiler::Destroyer::operator()(shaderc_compiler_t compiler) const noexcept {
    shaderc_compiler_release(compiler);
}

Compiler::Compiler()
        : compiler_(shaderc_compiler_initialize()),
          compile_options_(ShaderSourceLanguage::Glsl,
                           ShaderOptimizationLevel::Performance,
                           ShaderTargetEnv::Vulkan, ShaderEnvVersion::Vulkan_1_0) {
    if (!compiler_) {
        throw VkBaseError(
                "Could not initialize shaderc compiler. An unknown error occurred.");
    }
}

std::vector<uint32_t> Compiler::compile_shader(
        const std::string& input_filename, ShaderKind shader_kind,
        const std::string& entrypoint_name, const std::vector<Define>& defines) const {
    return compile_shader(read_file(input_filename), input_filename, shader_kind,
                          entrypoint_name, defines);
}

std::vector<uint32_t> Compiler::compile_shader(
        const std::string& input_filename, ShaderKind shader_kind,
        const std::vector<Define>& defines) const {
    return compile_shader(input_filename, shader_kind, "main", defines);
}

std::vector<uint32_t> Compiler::compile_shader(
        const std::string& source, const std::string& input_filename,
        ShaderKind shader_kind, const std::vector<Define>& defines) const {
    return compile_shader(source, input_filename, shader_kind, "main", defines);
}

std::vector<uint32_t> Compiler::compile_shader(
        const std::string& source, const std::string& input_filename,
        ShaderKind shader_kind, const std::string& entrypoint_name,
        const std::vector<Define>& defines) const {
    CompileOptions options = compile_options_;
    options.add_macro_definitions(defines.begin(), defines.end());

    CompilationResult compilation_result(*this, source, input_filename, shader_kind,
                                         entrypoint_name, options);

    ShaderCompilationStatus status = compilation_result.status();
    if (status != ShaderCompilationStatus::Success) {
        throw ShaderCCompileError(status, compilation_result.error_message());
    }

    std::string warnings;
    if (compilation_result.num_warnings() > 0) {
        warnings = compilation_result.error_message();
    }

    return compilation_result.spirv_code();
}

void CompilationResult::Destroyer::operator()(
        shaderc_compilation_result_t result) const noexcept {
    shaderc_result_release(result);
}

CompilationResult::CompilationResult(const Compiler& compiler,
                                     const std::string& source,
                                     const std::string& input_filename,
                                     ShaderKind shader_kind,
                                     const std::string& entrypoint_name,
                                     const CompileOptions& options)
        : compilation_result_(shaderc_compile_into_spv(
                  *compiler.compiler_, source.empty() ? nullptr : source.c_str(),
                  source.size(), static_cast<shaderc_shader_kind>(shader_kind),
                  input_filename.c_str(), entrypoint_name.c_str(),
                  *options.compile_options_)) {}

size_t CompilationResult::num_warnings() const noexcept {
    return shaderc_result_get_num_warnings(*compilation_result_);
}

size_t CompilationResult::num_errors() const noexcept {
    return shaderc_result_get_num_errors(*compilation_result_);
}

ShaderCompilationStatus CompilationResult::status() const {
    return static_cast<ShaderCompilationStatus>(
            shaderc_result_get_compilation_status(*compilation_result_));
}

const char* CompilationResult::error_message() const {
    return shaderc_result_get_error_message(*compilation_result_);
}

std::vector<uint32_t> CompilationResult::spirv_code() const {
    const uint32_t* first = reinterpret_cast<const uint32_t*>(
            shaderc_result_get_bytes(*compilation_result_));
    size_t size = shaderc_result_get_length(*compilation_result_) / sizeof(uint32_t);
    return std::vector<uint32_t>(first, first + size);
}
}  // namespace maseya::vkbase