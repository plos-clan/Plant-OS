// This code is released under the MIT License

#pragma once
#include "type_traits"

namespace cpp {

template <bool cond>
using enable_if = std::enable_if_t<cond>;

template <typename T>
inline constexpr bool is_float = std::is_floating_point_v<T>;

template <typename base, typename derived>
inline constexpr bool is_base_of = std::is_base_of_v<base, derived>;

template <typename T>
using remove_reference = std::remove_reference_t<T>;

template <typename T>
constexpr auto move(T &&t) noexcept -> remove_reference<T> && {
  return static_cast<remove_reference<T> &&>(t);
}

} // namespace cpp
