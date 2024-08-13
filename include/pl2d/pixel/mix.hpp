#pragma once
#include "pixel.hpp"

namespace pl2d {

// 假如源和目标都没有透明度
template <BasePixelTemplate>
void BasePixelT::mix_ratio(const BasePixelT &s, T k) {
  T2 sw = k;
  T2 dw = T_MAX - k;
  r     = (r * dw + s.r * sw) / T_MAX;
  g     = (g * dw + s.g * sw) / T_MAX;
  b     = (b * dw + s.b * sw) / T_MAX;
  a     = (a * dw + s.a * sw) / T_MAX;
}

// 假如源和目标都没有透明度
template <BasePixelTemplate>
auto BasePixelT::mix_ratio(const BasePixelT &c1, const BasePixelT &c2, T k) -> BasePixelT {
  T2 w1 = k;
  T2 w2 = T_MAX - k;
  return BasePixelT{
      (T)((c1.r * w1 + c2.r * w2) / T_MAX),
      (T)((c1.g * w1 + c2.g * w2) / T_MAX),
      (T)((c1.b * w1 + c2.b * w2) / T_MAX),
      (T)((c1.a * w1 + c2.a * w2) / T_MAX),
  };
}

// 假如源和目标都没有透明度
template <BasePixelTemplate>
template <typename U>
auto BasePixelT::mix_ratio(const BasePixelT &c1, const BasePixelT &c2, U k) -> BasePixelT
requires(std::is_floating_point_v<U> && !std::is_same_v<T, U>)
{
  return BasePixelT{
      (T)((U)c1.r * k + (U)c2.r * (1 - k)),
      (T)((U)c1.g * k + (U)c2.g * (1 - k)),
      (T)((U)c1.b * k + (U)c2.b * (1 - k)),
      (T)((U)c1.a * k + (U)c2.a * (1 - k)),
  };
}

// 假如源有透明度，目标没有透明度
template <BasePixelTemplate>
void BasePixelT::mix_opaque(const BasePixelT &s) {
  T2 sw = s.a;
  T2 dw = T_MAX - s.a;
  r     = (r * dw + s.r * sw) / T_MAX;
  g     = (g * dw + s.g * sw) / T_MAX;
  b     = (b * dw + s.b * sw) / T_MAX;
}

// 假如源有透明度，目标没有透明度
template <BasePixelTemplate>
auto BasePixelT::mix_opaque(const BasePixelT &c1, const BasePixelT &c2) -> BasePixelT {
  T2 w1 = T_MAX - c2.a;
  T2 w2 = c2.a;
  return BasePixelT{
      (T)((c1.r * w1 + c2.r * w2) / T_MAX),
      (T)((c1.g * w1 + c2.g * w2) / T_MAX),
      (T)((c1.b * w1 + c2.b * w2) / T_MAX),
      c1.a,
  };
}

// 假如源和目标都有透明度
template <BasePixelTemplate>
void BasePixelT::mix(const BasePixelT &s) {
  if (a == T_MAX) return mix_opaque(s);
  T2 _a = (T2)T_MAX * (a + s.a) - a * s.a;
  T2 sw = (T2)T_MAX * s.a;
  T2 dw = a * ((T2)T_MAX - s.a);
  r     = (r * dw + s.r * sw) / _a;
  g     = (g * dw + s.g * sw) / _a;
  b     = (b * dw + s.b * sw) / _a;
  a     = _a / T_MAX;
}

// 假如源和目标都有透明度
template <BasePixelTemplate>
auto BasePixelT::mix(const BasePixelT &c1, const BasePixelT &c2) -> BasePixelT {
  if (c1.a == T_MAX) return mix_opaque(c1, c2);
  T2 _a = (T2)T_MAX * (c1.a + c2.a) - c1.a * c2.a;
  T2 w1 = (T2)T_MAX * c1.a;
  T2 w2 = (T2)T_MAX * c2.a;
  return BasePixelT{
      (T)((c1.r * w1 + c2.r * w2) / _a),
      (T)((c1.g * w1 + c2.g * w2) / _a),
      (T)((c1.b * w1 + c2.b * w2) / _a),
      (T)(_a / T_MAX),
  };
}

} // namespace pl2d
