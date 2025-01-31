#pragma once
#include "define.hpp"
#include "using.hpp"
#include <define.h>
#include <libc-base.hpp>

// pixel 也可以处理颜色变换等

namespace pl2d {

dlimport auto gamma_correct(f32 x) -> f32; // 进行 gamma 矫正
dlimport auto gamma_correct(f64 x) -> f64; // 进行 gamma 矫正
dlimport auto reverse_gamma(f32 x) -> f32; // 反向 gamma 矫正
dlimport auto reverse_gamma(f64 x) -> f64; // 反向 gamma 矫正

template <BasePixelTemplate>
struct BasePixel {
  using TYPE                       = T;     // 储存颜色值的类型（无符号整数或浮点，有符号当作无符号
  using TYPE2                      = T2;    // 直接运算时的类型（至少大一倍防止溢出）
  using FLT_TYPE                   = FT;    // 转换成浮点数运算时的类型
  static constexpr auto TYPE_MAX   = T_MAX; // 最大值（浮点设置为 1）
  static constexpr auto TYPE_MAX_2 = T_MAX_2; // 对应有符号类型的最大值（浮点设置为 1）

#if COLOR_USE_BGR
  T b = 0, g = 0, r = 0, a = 0;
#else
  T r = 0, g = 0, b = 0, a = 0;
#endif

  INLINE_CONST   BasePixel() = default;
  INLINE_CONST   BasePixel(u32); // 0xRRGGBBAA 的格式来初始化颜色（仅当定义了 COLOR_READABLE_HEX）
  INLINE_CONST   BasePixel(const BasePixel &rgb, T a) : r(rgb.r), g(rgb.g), b(rgb.b), a(a) {}
  INLINE_CONST   BasePixel(T r, T g, T b) : r(r), g(g), b(b), a(T_MAX) {}
  INLINE_CONST   BasePixel(T r, T g, T b, T a) : r(r), g(g), b(b), a(a) {}
  INLINE_CONST   BasePixel(const BasePixel &)                    = default;
  INLINE_CONST   BasePixel(BasePixel &&) noexcept                = default;
  constexpr auto operator=(const BasePixel &) -> BasePixel &     = default;
  constexpr auto operator=(BasePixel &&) noexcept -> BasePixel & = default;

  template <_BasePixelTemplate>
  INLINE_CONST BasePixel(const _BasePixelT &p);

  INLINE_CONST explicit operator u32() const;

  INLINE_CONST auto operator==(const BasePixel &p) const -> bool {
    return r == p.r && g == p.g && b == p.b && a == p.a;
  }

  INLINE_CONST auto copy() const -> BasePixel {
    return *this;
  }

  INLINE_CONST auto clamp() const -> BasePixel {
    return {
        cpp::clamp(r, T(0), T(T_MAX)),
        cpp::clamp(g, T(0), T(T_MAX)),
        cpp::clamp(b, T(0), T(T_MAX)),
        cpp::clamp(a, T(0), T(T_MAX)),
    };
  }
  INLINE_CONST auto clamp(T low, T high) const -> BasePixel {
    return {
        cpp::clamp(r, low, high),
        cpp::clamp(g, low, high),
        cpp::clamp(b, low, high),
        cpp::clamp(a, low, high),
    };
  }

  // 元素逐个运算
  constexpr auto operator+(const BasePixel &s) const -> BasePixel;
  constexpr auto operator+=(const BasePixel &s) -> BasePixel &;
  constexpr auto operator-(const BasePixel &s) const -> BasePixel;
  constexpr auto operator-=(const BasePixel &s) -> BasePixel &;
  constexpr auto operator*(const BasePixel &s) const -> BasePixel;
  constexpr auto operator*=(const BasePixel &s) -> BasePixel &;
  constexpr auto operator/(const BasePixel &s) const -> BasePixel;
  constexpr auto operator/=(const BasePixel &s) -> BasePixel &;
  template <typename U>
  requires(cpp::is_num<U>)
  auto operator+(U s) const -> BasePixel;
  template <typename U>
  requires(cpp::is_num<U>)
  auto operator+=(U s) -> BasePixel &;
  template <typename U>
  requires(cpp::is_num<U>)
  auto operator-(U s) const -> BasePixel;
  template <typename U>
  requires(cpp::is_num<U>)
  auto operator-=(U s) -> BasePixel &;
  template <typename U>
  requires(cpp::is_num<U>)
  auto operator*(U s) const -> BasePixel;
  template <typename U>
  requires(cpp::is_num<U>)
  auto operator*=(U s) -> BasePixel &;
  template <typename U>
  requires(cpp::is_num<U>)
  auto operator/(U s) const -> BasePixel;
  template <typename U>
  requires(cpp::is_num<U>)
  auto operator/=(U s) -> BasePixel &;

  template <typename U>
  requires(cpp::is_num<U>)
  friend auto operator+(U s, const BasePixel &p) -> BasePixel {
    return p + s;
  }
  template <typename U>
  requires(cpp::is_num<U>)
  friend auto operator-(U s, const BasePixel &p) -> BasePixel {
    return BasePixel(s, s, s, s) - p;
  }
  template <typename U>
  requires(cpp::is_num<U>)
  friend auto operator*(U s, const BasePixel &p) -> BasePixel {
    return p * s;
  }
  template <typename U>
  requires(cpp::is_num<U>)
  friend auto operator/(U s, const BasePixel &p) -> BasePixel {
    return BasePixel(s, s, s, s) / p;
  }

  // 计算颜色的差值
  CONST auto diff(const BasePixel &p) -> T {
    T dr = cpp::diff(r, p.r);
    T dg = cpp::diff(g, p.g);
    T db = cpp::diff(b, p.b);
    return cpp::max(dr, dg, db);
  }

  // 按比例进行颜色混合函数
  HOT void        mix_ratio(const BasePixel &s, FT k);
  HOT static auto mix_ratio(const BasePixel &c1, const BasePixel &c2, FT k) -> BasePixel;
  // 背景色不透明的混合函数
  HOT void        mix_opaque(const BasePixel &s);
  HOT static auto mix_opaque(const BasePixel &c1, const BasePixel &c2) -> BasePixel;
  // 通用的混合函数
  HOT void        mix(const BasePixel &s);
  HOT static auto mix(const BasePixel &c1, const BasePixel &c2) -> BasePixel;

  // 亮度
  auto brightness() const -> T;
  auto grayscale() const -> BasePixel;

  auto gamma_correct() const -> BasePixel; // 进行 gamma 矫正
  auto reverse_gamma() const -> BasePixel; // 反向 gamma 矫正

  INLINE_CONST auto to_u8() const -> PixelB {
    return *this;
  }
  INLINE_CONST auto to_u16() const -> PixelS {
    return *this;
  }
  INLINE_CONST auto to_u32() const -> PixelI {
    return *this;
  }
  INLINE_CONST auto to_f32() const -> PixelF {
    return *this;
  }
  INLINE_CONST auto to_f64() const -> PixelD {
    return *this;
  }

  INLINE_CONST auto to_float() const -> BasePixel<FT, FT, FT, 1, 1> {
    return *this;
  }

  auto RGB2Grayscale() -> BasePixel &;
  auto RGB2HSV() -> BasePixel &;
  auto HSV2RGB() -> BasePixel &;
  auto RGB2HSL() -> BasePixel &;
  auto HSL2RGB() -> BasePixel &;
  auto RGB2XYZ() -> BasePixel &;
  auto XYZ2RGB() -> BasePixel &;
  auto XYZ2LAB() -> BasePixel &;
  auto LAB2XYZ() -> BasePixel &;
  auto RGB2LAB() -> BasePixel &;
  auto LAB2RGB() -> BasePixel &;
  auto XYZ2LUV() -> BasePixel &;
  auto LUV2XYZ() -> BasePixel &;
  auto RGB2LUV() -> BasePixel &;
  auto LUV2RGB() -> BasePixel &;

  INLINE_CONST auto rgb() const -> BasePixel {
    return BasePixel(*this, T_MAX);
  }
  INLINE_CONST auto alpha() const -> BasePixel {
    return BasePixel(0, 0, 0, a);
  }
  INLINE_CONST auto hsv() const -> BasePixel {
    return BasePixel(*this).RGB2HSV();
  }
  INLINE_CONST auto hsl() const -> BasePixel {
    return BasePixel(*this).RGB2HSL();
  }
  INLINE_CONST auto xyz() const -> BasePixel {
    return BasePixel(*this).RGB2XYZ();
  }
  INLINE_CONST auto lab() const -> BasePixel {
    return BasePixel(*this).RGB2LAB();
  }
  INLINE_CONST auto luv() const -> BasePixel {
    return BasePixel(*this).RGB2LUV();
  }

  inline_const auto rgb(T r, T g, T b) -> BasePixel {
    return BasePixel(r, g, b);
  }
  inline_const auto rgba(T r, T g, T b, T a) -> BasePixel {
    return BasePixel(r, g, b, a);
  }
  inline_const auto hsv(T h, T s, T v) -> BasePixel {
    return BasePixel(h, s, v).HSV2RGB();
  }
  inline_const auto hsl(T h, T s, T l) -> BasePixel {
    return BasePixel(h, s, l).HSL2RGB();
  }
  inline_const auto xyz(T x, T y, T z) -> BasePixel {
    return BasePixel(x, y, z).XYZ2RGB();
  }
  inline_const auto lab(T l, T a, T b) -> BasePixel {
    return BasePixel(l, a, b).LAB2RGB();
  }
  inline_const auto luv(T l, T u, T v) -> BasePixel {
    return BasePixel(l, u, v).LUV2RGB();
  }
};

} // namespace pl2d
