#pragma once
#include "../point.hpp"
#include <define.h>

namespace pl2d {

template <typename T>
struct BaseTriangle {
  T x1 = 0, y1 = 0;
  T x2 = 0, y2 = 0;
  T x3 = 0, y3 = 0;

  INLINE_CONST BaseTriangle() = default;
  INLINE_CONST BaseTriangle(T x1, T y1, T x2, T y2, T x3, T y3) {
    this->x1 = x1, this->y1 = y1;
    this->x2 = x2, this->y2 = y2;
    this->x3 = x3, this->y3 = y3;
  }
  INLINE_CONST BaseTriangle(const BaseTriangle &)     = default;
  INLINE_CONST BaseTriangle(BaseTriangle &&) noexcept = default;

  constexpr auto operator=(const BaseTriangle &) -> BaseTriangle &     = default;
  constexpr auto operator=(BaseTriangle &&) noexcept -> BaseTriangle & = default;

  INLINE_CONST auto translate(T x, T y) -> BaseTriangle & {
    x1 += x;
    x2 += x;
    x3 += x;
    y1 += y;
    y2 += y;
    y3 += y;
    return *this;
  }
};

using TriangleI = BaseTriangle<i32>;
using TriangleF = BaseTriangle<f32>;
using TriangleD = BaseTriangle<f64>;
using Triangle  = TriangleI;

} // namespace pl2d
