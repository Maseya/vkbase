#pragma once

#include <algorithm>
#include <cctype>
#include <cwctype>
#include <memory>
#include <string>

namespace maseya::vkbase {
template <class Elem, class Traits = std::char_traits<Elem>,
          class Alloc = std::allocator<Elem>>
std::basic_string<Elem, Traits, Alloc> to_lower(
        std::basic_string<Elem, Traits, Alloc>&& s) {
    std::basic_string<Elem, Traits, Alloc> result(std::move(s));
    if constexpr (std::is_same_v<Elem, char>) {
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
    } else {
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned wchar_t c) { return std::towlower(c); });
    }

    return result;
}

template <class Elem, class Traits = std::char_traits<Elem>,
          class Alloc = std::allocator<Elem>>
std::basic_string<Elem, Traits, Alloc> to_lower(
        const std::basic_string<Elem, Traits, Alloc>& s) {
    std::basic_string<Elem, Traits, Alloc> copy(s);
    return to_lower(std::move(copy));
}
}  // namespace maseya::vkbase