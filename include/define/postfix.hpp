// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#include "type.hpp"

constexpr auto operator""_B(unsigned long long x) -> usize {
  return x;
}

constexpr auto operator""_KiB(unsigned long long x) -> usize {
  return x * 1024;
}

constexpr auto operator""_MiB(unsigned long long x) -> usize {
  return x * 1024 * 1024;
}
