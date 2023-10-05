#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace maseya::vkbase {
template <class T>
std::vector<T> read_file(const char* path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    std::streamsize size = file.tellg();
    if (size == 0) {
        return {};
    }

    std::vector<T> result(static_cast<size_t>(1 + ((size - 1) / sizeof(T))));
    file.seekg(0);
    file.read(reinterpret_cast<char*>(result.data()), size);
    file.close();

    return result;
}

template <class T>
std::vector<T> read_file(const std::string& path) {
    return read_file(path.c_str());
}

std::string read_file(const char* path);

std::string read_file(const std::string& path) { return read_file(path.c_str()); }
}  // namespace maseya::vkbase
