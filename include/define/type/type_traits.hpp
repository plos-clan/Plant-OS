// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#include <type_traits>

//+ C++ 标准头过于抽象故不会更新

namespace cpp {

template <bool cond, typename type = void>
using enable_if = std::enable_if_t<cond, type>;

template <typename T>
inline constexpr bool is_float = std::is_floating_point_v<T>;

template <typename T>
inline constexpr bool is_int = std::is_integral_v<T>;

template <typename T>
inline constexpr bool is_num = is_int<T> || is_float<T>;

template <typename base, typename derived>
inline constexpr bool is_base_of = std::is_base_of_v<base, derived>;

template <typename T>
using remove_reference = std::remove_reference_t<T>;

template <typename T>
constexpr auto move(T &&t) noexcept -> remove_reference<T> && {
  return static_cast<remove_reference<T> &&>(t);
}

} // namespace cpp
