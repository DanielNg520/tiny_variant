#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <new>
#include <exception>

namespace tiny {

struct bad_variant_access : std::exception {
    const char* what() const noexcept override;
};

template <class... Ts>
class variant {
public:
    // --- member types ---
    static constexpr std::size_t npos = static_cast<std::size_t>(-1);

    // --- constructors ---
    variant() noexcept(std::is_nothrow_default_constructible_v<std::tuple_element_t<0, std::tuple<Ts...>>>);

    variant(const variant& other);
    variant(variant&& other) noexcept;

    template <class T, class = std::enable_if_t<(std::is_same_v<std::decay_t<T>, Ts> || ...)>>
    variant(T&& value);

    // --- destructor ---
    ~variant();

    // --- assignment ---
    variant& operator=(const variant& other);
    variant& operator=(variant&& other) noexcept;

    template <class T, class = std::enable_if_t<(std::is_same_v<std::decay_t<T>, Ts> || ...)>>
    variant& operator=(T&& value);

    // --- observers ---
    std::size_t index() const noexcept;
    bool valueless_by_exception() const noexcept;

    template <class T>
    bool holds_alternative() const noexcept;

    // --- modifiers ---
    void swap(variant& other) noexcept;

    template <class T, class... Args,
              class = std::enable_if_t<(std::is_same_v<T, Ts> || ...)>>
    T& emplace(Args&&... args);

    // --- access ---
    template <class T>
    T& get();

    template <class T>
    const T& get() const;

    template <std::size_t I>
    auto& get();

    template <std::size_t I>
    const auto& get() const;

private:
    // internal helpers (declared, not defined here)
    void destroy() noexcept;

    template <std::size_t I, class... Args>
    void construct_in_place(Args&&... args);

    // storage: big enough and aligned enough for all Ts...
    static constexpr std::size_t storage_size = /* impl detail */;
    static constexpr std::size_t storage_align = /* impl detail */;

    alignas(/*storage_align*/) unsigned char storage_[/*storage_size*/];
    std::size_t index_ = npos; // npos means valueless
};

// --- non-member utilities ---
template <class T, class... Ts>
bool holds_alternative(const variant<Ts...>& v) noexcept;

template <class T, class... Ts>
T& get(variant<Ts...>& v);

template <class T, class... Ts>
const T& get(const variant<Ts...>& v);

template <std::size_t I, class... Ts>
auto& get(variant<Ts...>& v);

template <std::size_t I, class... Ts>
const auto& get(const variant<Ts...>& v);

// Optional (high value): visitor
template <class Visitor, class... Ts>
decltype(auto) visit(Visitor&& vis, variant<Ts...>& v);

template <class Visitor, class... Ts>
decltype(auto) visit(Visitor&& vis, const variant<Ts...>& v);

} // namespace tiny
