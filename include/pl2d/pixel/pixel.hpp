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

  BasePixel() = default;
  BasePixel(u32); // 0xRRGGBBAA 的格式来初始化颜色（仅当定义了 COLOR_READABLE_HEX）
  BasePixel(T r, T g, T b) : r(r), g(g), b(b), a(T_MAX) {}
  BasePixel(T r, T g, T b, T a) : r(r), g(g), b(b), a(a) {}
  BasePixel(const BasePixel &)                         = default;
  BasePixel(BasePixel &&) noexcept                     = default;
  auto operator=(const BasePixel &) -> BasePixel &     = default;
  auto operator=(BasePixel &&) noexcept -> BasePixel & = default;

  template <_BasePixelTemplate>
  BasePixel(const _BasePixelT &p);

  operator u32() const;

  auto operator==(const BasePixel &p) const -> bool {
    return r == p.r && g == p.g && b == p.b && a == p.a;
  }

  // 元素逐个运算
  auto operator+(const BasePixel &s) const -> BasePixel;
  auto operator+=(const BasePixel &s) -> BasePixel &;
  auto operator-(const BasePixel &s) const -> BasePixel;
  auto operator-=(const BasePixel &s) -> BasePixel &;
  template <typename U>
  auto operator*(U s) const -> BasePixel
  requires(cpp::is_float<U>);
  template <typename U>
  auto operator*=(U s) -> BasePixel &requires(cpp::is_float<U>);
  template <typename U>
  auto operator/(U s) const -> BasePixel
  requires(cpp::is_float<U>);
  template <typename U>
  auto operator/=(U s) -> BasePixel &requires(cpp::is_float<U>);

  // 计算颜色的差值
  auto diff(const BasePixel &p) -> T;

  // 按比例进行颜色混合函数
  void        mix_ratio(const BasePixel &s, T k);
  static auto mix_ratio(const BasePixel &c1, const BasePixel &c2, T k) -> BasePixel;
  template <typename U>
  static auto mix_ratio(const BasePixel &c1, const BasePixel &c2, U k) -> BasePixel
  requires(cpp::is_float<U> && !std::is_same_v<T, U>);
  // 背景色不透明的混合函数
  void        mix_opaque(const BasePixel &s);
  static auto mix_opaque(const BasePixel &c1, const BasePixel &c2) -> BasePixel;
  // 通用的混合函数
  void        mix(const BasePixel &s);
  static auto mix(const BasePixel &c1, const BasePixel &c2) -> BasePixel;

  // 亮度
  auto brightness() const -> T;
  auto grayscale() const -> BasePixel;

  auto gamma_correct() const -> BasePixel; // 进行 gamma 矫正
  auto reverse_gamma() const -> BasePixel; // 反向 gamma 矫正

  auto to_u8() const -> PixelB {
    return *this;
  }
  auto to_u16() const -> PixelS {
    return *this;
  }
  auto to_u32() const -> PixelI {
    return *this;
  }
  auto to_f32() const -> PixelF {
    return *this;
  }
  auto to_f64() const -> PixelD {
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
};

template <BasePixelTemplate>
auto BasePixelT::diff(const BasePixel &p) -> T {
  T dr = cpp::diff(r, p.r);
  T dg = cpp::diff(g, p.g);
  T db = cpp::diff(b, p.b);
  return cpp::max(dr, dg, db);
}

} // namespace pl2d
