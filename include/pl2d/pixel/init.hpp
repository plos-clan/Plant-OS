#pragma once
#include "pixel.hpp"

namespace pl2d {

template <BasePixelTemplate>
INLINE_CONST BasePixelT::BasePixel(u32 c) {
  DESTRUCT_RGBA(c, r, g, b, a)
  if constexpr (cpp::is_float<T>) {
    this->r = r / (T)255;
    this->g = g / (T)255;
    this->b = b / (T)255;
    this->a = a / (T)255;
  } else {
    this->r = (u64)r * T_MAX / 255;
    this->g = (u64)g * T_MAX / 255;
    this->b = (u64)b * T_MAX / 255;
    this->a = (u64)a * T_MAX / 255;
  }
}

template <BasePixelTemplate>
template <_BasePixelTemplate>
INLINE_CONST BasePixelT::BasePixel(const _BasePixelT &p) {
  if constexpr (cpp::is_float<T> && cpp::is_float<_T>) {
    r = (T)p.r, g = (T)p.g, b = (T)p.b, a = (T)p.a;
  } else if constexpr (cpp::is_float<T> && !cpp::is_float<_T>) {
    if constexpr (sizeof(FT) >= sizeof(_FT)) {
      r = p.r / (T)_T_MAX, g = p.g / (T)_T_MAX, b = p.b / (T)_T_MAX, a = p.a / (T)_T_MAX;
    } else {
      r = p.r / (_FT)_T_MAX, g = p.g / (_FT)_T_MAX, b = p.b / (_FT)_T_MAX, a = p.a / (_FT)_T_MAX;
    }
  } else if constexpr (!cpp::is_float<T> && cpp::is_float<_T>) {
    r = cpp::clamp((FT)p.r, (FT)0, (FT)1) * T_MAX, g = cpp::clamp((FT)p.g, (FT)0, (FT)1) * T_MAX;
    b = cpp::clamp((FT)p.b, (FT)0, (FT)1) * T_MAX, a = cpp::clamp((FT)p.a, (FT)0, (FT)1) * T_MAX;
  } else {
    r = (u64)p.r * T_MAX / _T_MAX, g = (u64)p.g * T_MAX / _T_MAX;
    b = (u64)p.b * T_MAX / _T_MAX, a = (u64)p.a * T_MAX / _T_MAX;
  }
}

template <BasePixelTemplate>
INLINE_CONST BasePixelT::operator u32() const {
  const PixelB p = *this;
  return RGBA(p.r, p.g, p.b, p.a);
}

} // namespace pl2d
