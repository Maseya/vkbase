#include "VulkanError.hxx"

namespace maseya::vkbase {
static std::string get_win_api_error(DWORD code);

VkBaseError::VkBaseError(const char* message) : std::runtime_error(message) {}
VkBaseError::VkBaseError(const std::string& message) : VkBaseError(message.c_str()) {}

void assert_result(VkResult result) {
    if (result < 0) {
        throw VulkanApiError(result);
    }
}

VulkanApiError::VulkanApiError(VkResult result)
        : VulkanApiError(result, get_error_message(result)) {}
VulkanApiError::VulkanApiError(VkResult result, const char* message)
        : VkBaseError(message), result_(result) {}
VulkanApiError::VulkanApiError(VkResult result, const std::string& message)
        : VulkanApiError(result, message.c_str()) {}

VulkanWinApiError::VulkanWinApiError() : VulkanWinApiError(GetLastError()) {
    SetLastError(0);
}
VulkanWinApiError::VulkanWinApiError(DWORD code)
        : VulkanWinApiError(code, get_win_api_error(code)) {}
VulkanWinApiError::VulkanWinApiError(DWORD code, const char* message)
        : VkBaseError(message), code_(code) {}
VulkanWinApiError::VulkanWinApiError(DWORD code, const std::string& message)
        : VulkanWinApiError(code, message.c_str()) {}

static std::string get_win_api_error(DWORD code) {
    constexpr static DWORD format_flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                          FORMAT_MESSAGE_FROM_SYSTEM |
                                          FORMAT_MESSAGE_IGNORE_INSERTS;
    LPSTR buffer{};
    if (FormatMessageA(format_flags, NULL, code, 0, reinterpret_cast<LPSTR>(&buffer), 0,
                       NULL) == 0) {
        throw VulkanWinApiError();
    }

    std::string result(buffer);
    LocalFree(buffer);
    return result;
}
}  // namespace maseya::vkbase