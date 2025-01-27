#pragma once
#include "pixel.hpp"

namespace pl2d {

#if COLOR_BETTER_MIX && (COLOR_FAST_MIX || COLOR_FASTIST_MIX)
#  error "Cannot enable COLOR_BETTER_MIX with COLOR_FAST_MIX or COLOR_FASTIST_MIX"
#endif

#if COLOR_FAST_MIX && COLOR_FASTIST_MIX
#  warning "COLOR_FAST_MIX and COLOR_FASTIST_MIX are both enabled, use COLOR_FASTIST_MIX"
#endif

#if COLOR_FASTIST_MIX
// 将 T_MAX + 1 凑个整，便于编译器优化  特判：浮点数不加 1
#  define T_MAX_ (cpp::is_float<T> ? T_MAX : (T2)T_MAX + 1)
#else
#  define T_MAX_ T_MAX
#endif

// 假如源和目标都没有透明度
template <BasePixelTemplate>
void BasePixelT::mix_ratio(const BasePixelT &s, T k) {
  T2 sw = k;
  T2 dw = T_MAX_ - k;
  r     = (r * dw + s.r * sw) / T_MAX_;
  g     = (g * dw + s.g * sw) / T_MAX_;
  b     = (b * dw + s.b * sw) / T_MAX_;
  a     = (a * dw + s.a * sw) / T_MAX_;
}

// 假如源和目标都没有透明度
template <BasePixelTemplate>
auto BasePixelT::mix_ratio(const BasePixelT &c1, const BasePixelT &c2, T k) -> BasePixelT {
  T2 w1 = k;
  T2 w2 = T_MAX_ - k;
  return BasePixelT{
      (T)((c1.r * w1 + c2.r * w2) / T_MAX_),
      (T)((c1.g * w1 + c2.g * w2) / T_MAX_),
      (T)((c1.b * w1 + c2.b * w2) / T_MAX_),
      (T)((c1.a * w1 + c2.a * w2) / T_MAX_),
  };
}

// 假如源和目标都没有透明度
template <BasePixelTemplate>
template <typename U>
requires(cpp::is_float<U> && !std::is_same_v<T, U>)
auto BasePixelT::mix_ratio(const BasePixelT &c1, const BasePixelT &c2, U k) -> BasePixelT {
  return BasePixelT{
      (T)((U)c1.r * k + (U)c2.r * (1 - k)),
      (T)((U)c1.g * k + (U)c2.g * (1 - k)),
      (T)((U)c1.b * k + (U)c2.b * (1 - k)),
      (T)((U)c1.a * k + (U)c2.a * (1 - k)),
  };
}

// 假如源有透明度，目标没有透明度 (目标有透明度为未定义行为)
template <BasePixelTemplate>
void BasePixelT::mix_opaque(const BasePixelT &s) {
  if (s.a == T_MAX) {
    *this = s;
    return;
  }
  T2 sw = s.a;
  T2 dw = T_MAX_ - s.a;
  r     = (r * dw + s.r * sw) / T_MAX_;
  g     = (g * dw + s.g * sw) / T_MAX_;
  b     = (b * dw + s.b * sw) / T_MAX_;
}

// 假如源有透明度，目标没有透明度 (目标有透明度为未定义行为)
template <BasePixelTemplate>
auto BasePixelT::mix_opaque(const BasePixelT &c1, const BasePixelT &c2) -> BasePixelT {
  if (c2.a == T_MAX) return c2;
  T2 w1 = T_MAX_ - c2.a;
  T2 w2 = c2.a;
  return BasePixelT{
      (T)((c1.r * w1 + c2.r * w2) / T_MAX_),
      (T)((c1.g * w1 + c2.g * w2) / T_MAX_),
      (T)((c1.b * w1 + c2.b * w2) / T_MAX_),
      c1.a,
  };
}

#undef T_MAX_

#if COLOR_FAST_MIX || COLOR_FASTIST_MIX

template <BasePixelTemplate>
void BasePixelT::mix(const BasePixelT &s) {
  return mix_opaque(s);
}

template <BasePixelTemplate>
auto BasePixelT::mix(const BasePixelT &c1, const BasePixelT &c2) -> BasePixelT {
  return mix_opaque(c1, c2);
}

#else

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

#endif

} // namespace pl2d
