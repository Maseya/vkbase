#pragma once

#include <cstddef>
#include <utility>

namespace maseya::vkbase {
template <class T, class Destroyer>
class UniqueObject {
public:
    using element_type = T;
    using destroyer_type = Destroyer;

    constexpr UniqueObject() noexcept : destroyer_(), value_() {}
    constexpr UniqueObject(std::nullptr_t) noexcept : UniqueObject() {}

    explicit UniqueObject(element_type value) noexcept : destroyer_(), value_(value) {}

    UniqueObject(element_type value, const destroyer_type& destroyer)
            : destroyer_(destroyer), value_(value) {}
    UniqueObject(element_type value, destroyer_type&& destroyer) noexcept
            : destroyer_(std::move(destroyer)), value_(value) {}

    template <typename... Args>
    UniqueObject(element_type value, Args&&... args)
            : UniqueObject(value, destroyer_type(std::forward<Args>(args)...)) {}

    UniqueObject(const UniqueObject&) = delete;
    UniqueObject(UniqueObject&& rhs) noexcept
            : destroyer_(std::move(rhs.destroyer_)),
              value_(std::exchange(rhs.value_, {})) {}

    ~UniqueObject() { reset(); }

    UniqueObject& operator=(const UniqueObject&) = delete;
    UniqueObject& operator=(UniqueObject&& rhs) noexcept {
        std::swap(destroyer_, rhs.destroyer_);
        std::swap(value_, rhs.value_);
        return *this;
    }

    UniqueObject& operator=(std::nullptr_t) noexcept { reset(); }

    element_type release() noexcept { return std::exchange(value_, {}); }

    void reset(element_type value = {}) noexcept {
        if (value_) {
            destroyer_(value_);
        }

        value_ = value;
    }

    element_type& get() noexcept { return value_; }
    const element_type& get() const noexcept { return value_; }

    destroyer_type& get_destroyer() { return destroyer_; }
    const destroyer_type& get_destroyer() const { return destroyer_; }

    explicit operator bool() const noexcept { return value_; }

    element_type& operator*() noexcept { return value_; }
    const element_type& operator*() const noexcept { return value_; }

    element_type* operator->() noexcept { return &value_; }
    const element_type* operator->() const noexcept { return &value_; }

private:
    destroyer_type destroyer_;
    element_type value_;
};

template <class T1, class D1, class T2, class D2>
constexpr bool operator==(const UniqueObject<T1, D1>& x,
                          const UniqueObject<T2, D2>& y) {
    return x.get() == y.get();
}
template <class T1, class D1, class T2, class D2>
constexpr bool operator!=(const UniqueObject<T1, D1>& x,
                          const UniqueObject<T2, D2>& y) {
    return !(x == y);
}
template <class T, class D>
constexpr bool operator==(const UniqueObject<T, D>& x, std::nullptr_t) noexcept {
    return !x.get();
}
template <class T, class D>
constexpr bool operator==(std::nullptr_t, const UniqueObject<T, D>& y) noexcept {
    return y == nullptr;
}
template <class T, class D>
constexpr bool operator!=(const UniqueObject<T, D>& x, std::nullptr_t) noexcept {
    return !(x == nullptr);
}
template <class T, class D>
constexpr bool operator!=(std::nullptr_t, const UniqueObject<T, D>& y) noexcept {
    return !(y == nullptr);
}
}  // namespace maseya::vkbase