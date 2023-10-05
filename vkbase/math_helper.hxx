#pragma once

#include <cstdint>
#include <functional>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace maseya {
template <class T, class Hasher = std::hash<T>>
inline void hash_combine(std::size_t& seed, const T& v, const Hasher& hasher) {
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <class T, class Hasher = std::hash<T>,
          std::enable_if_t<std::is_default_constructible_v<Hasher>, int> = 0>
inline void hash_combine(std::size_t& seed, const T& v) {
    hash_combine(seed, v, Hasher());
}

template <class InputIt, class Hasher = std::hash<std::remove_reference_t<
                                 decltype(*std::declval<InputIt>())>>>
inline void hash_combine(std::size_t& seed, InputIt first, InputIt last,
                         const Hasher& hasher) {
    for (auto it = first; it != last; ++it) {
        hash_combine(seed, *it, hasher);
    }
}

template <class InputIt,
          class Hasher = std::hash<
                  std::remove_reference_t<decltype(*std::declval<InputIt>())>>,
          std::enable_if_t<std::is_default_constructible_v<Hasher>, int> = 0>
inline void hash_combine(std::size_t& seed, InputIt first, InputIt last) {
    hash_combine(seed, first, last, Hasher());
}

template <class T, class Hasher = std::hash<T>>
inline void hash_combine(std::size_t& seed, const std::vector<T>& items,
                         const Hasher& hasher) {
    hash_combine(seed, items.begin(), items.end(), hasher);
}

template <class T, class Hasher = std::hash<T>,
          std::enable_if_t<std::is_default_constructible_v<Hasher>, int> = 0>
inline void hash_combine(std::size_t& seed, const std::vector<T>& items) {
    hash_combine(seed, items.begin(), items.end(), Hasher());
}

template <class T, size_t N, class Hasher = std::hash<T>>
inline void hash_combine(std::size_t& seed, const T (&items)[N], const Hasher& hasher) {
    hash_combine(seed, std::begin(items), std::end(items), hasher);
}

template <class T, size_t N, class Hasher = std::hash<T>,
          std::enable_if_t<std::is_default_constructible_v<Hasher>, int> = 0>
inline void hash_combine(std::size_t& seed, const T (&items)[N]) {
    hash_combine(seed, items, Hasher());
}

template <class T, class Hasher = std::hash<T>>
inline void hash_combine(std::size_t& seed, const T* items, std::size_t count,
                         const Hasher& hasher) {
    hash_combine(seed, items, items + count, hasher);
}

template <class T, class Hasher = std::hash<T>,
          std::enable_if_t<std::is_default_constructible_v<Hasher>, int> = 0>
inline void hash_combine(std::size_t& seed, const T* items, std::size_t count) {
    hash_combine(seed, items, items + count, Hasher());
}

template <class T, class Hasher = std::hash<T>>
inline void hash_combine_invariant(std::size_t& seed, const T& v,
                                   const Hasher& hasher) {
    seed ^= hasher(v) + 0x9e3779b9;
}

template <class T, class Hasher = std::hash<T>, class Eq = std::equal_to<T>,
          std::enable_if_t<std::is_default_constructible_v<Hasher>, int> = 0>
inline void hash_combine_invariant(std::size_t& seed, const T& v) {
    hash_combine_invariant(seed, v, Hasher());
}

template <class InputIt, class Hasher = std::hash<std::remove_reference_t<
                                 decltype(*std::declval<InputIt>())>>>
inline void hash_combine_invariant(std::size_t& seed, InputIt first, InputIt last,
                                   const Hasher& hasher) {
    for (auto it = first; it != last; ++it) {
        hash_combine_invariant(seed, *it, hasher);
    }
}

template <class InputIt,
          class Hasher = std::hash<
                  std::remove_reference_t<decltype(*std::declval<InputIt>())>>,
          std::enable_if_t<std::is_default_constructible_v<Hasher>, int> = 0>
inline void hash_combine_invariant(std::size_t& seed, InputIt first, InputIt last) {
    hash_combine_invariant(seed, first, last, Hasher());
}

template <class T, class Hasher = std::hash<T>, class Equal = std::equal_to<T>>
inline void hash_combine_invariant(std::size_t& seed,
                                   const std::unordered_set<T, Hasher, Equal>& items,
                                   const Hasher& hasher) {
    hash_combine_invariant(seed, items.begin(), items.end(), hasher);
}

template <class T, class Hasher = std::hash<T>, class Equal = std::equal_to<T>,
          std::enable_if_t<std::is_default_constructible_v<Hasher>, int> = 0>
inline void hash_combine_invariant(std::size_t& seed,
                                   const std::unordered_set<T, Hasher, Equal>& items) {
    hash_combine_invariant(seed, items.begin(), items.end(), Hasher());
}
}  // namespace maseya