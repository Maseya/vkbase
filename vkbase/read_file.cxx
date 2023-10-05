#include "read_file.hxx"

namespace maseya::vkbase {
std::string read_file(const char* path) {
    std::ifstream file(path, std::ios::ate);
    std::streamsize size = file.tellg();
    if (size == 0) return {};

    std::string result(static_cast<size_t>(size), ' ');
    file.seekg(0);
    file.read(result.data(), size);
    file.close();

    return result;
}
}  // namespace maseya::vkbase