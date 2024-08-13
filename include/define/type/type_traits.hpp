#pragma once
#include "type_traits"

namespace cpp {

template <bool cond>
using enable_if = std::enable_if_t<cond>;

template <typename base, typename derived>
inline constexpr bool is_base_of = std::is_base_of_v<base, derived>;

} // namespace cpp
