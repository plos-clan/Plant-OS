#pragma once
#include "../pixel.hpp"
#include "../points.hpp"

namespace pl2d {

// 显式模板实例化
#define BaseTextureInstantiation                                                                   \
  template class dlexport BaseTexture<PixelB>;                                                     \
  template class dlexport BaseTexture<PixelS>;                                                     \
  template class dlexport BaseTexture<PixelF>;                                                     \
  template class dlexport BaseTexture<PixelD>;

// BaseTexture 参数约定：
//   图像宽度与高度必须是 2 的整数倍
//   否则请勿调用与 fft 相关的功能
//
// 所有返回 BaseTexture & 的函数返回的都是结构体自身
// 可以用来实现链式调用

template <typename T>
struct BaseTexture;

using TextureB = BaseTexture<PixelB>;
using TextureS = BaseTexture<PixelS>;
using TextureF = BaseTexture<PixelF>;
using TextureD = BaseTexture<PixelD>;
using Texture  = TextureB;

template <typename T>
struct BaseTexture {
  // 可以是内部通过 malloc 分配，也可以是外部数据
  T *pixels sized_by(width *height) = null;

  bool own_pixels      = false; // pixels 是否为该结构体所有
  bool refcnted_pixels = false; // pixels 是否使用引用计数
  bool copy_on_write   = false; // TODO 是否在写时拷贝对象
  bool use_alpha       = true;  // TODO 是否使用 alpha 通道
  u32  width           = 0;     // 宽度
  u32  height          = 0;     // 高度
  u32  pitch           = 0;     // 每行实际的像素数，超过宽度的部分忽略
  u32  size            = 0;     // 储存 height * pitch 而不是占用的字节数
  u32  alloced_size    = 0;     // 储存实际分配的大小 像素数而不是字节数

  free_t free_pixels = null; // 释放用的函数

  BaseTexture() = default;
  BaseTexture(u32 width, u32 height);                       // 创建一个空的纹理
  BaseTexture(u32 width, u32 height, u32 pitch);            // 创建一个空的纹理
  BaseTexture(T *pixels, u32 width, u32 height);            // 使用外部数据创建纹理
  BaseTexture(T *pixels, u32 width, u32 height, u32 pitch); // 使用外部数据创建纹理
  BaseTexture(const BaseTexture &) = delete;                // 隐式地复制是不允许的
  BaseTexture(BaseTexture &&) noexcept;                     // 移动是可以的
  ~BaseTexture() noexcept;
  auto operator=(const BaseTexture &) -> BaseTexture & = delete; // 隐式地复制是不允许的
  auto operator=(BaseTexture &&) noexcept -> BaseTexture &;      // 移动是可以的

  auto reset() -> BaseTexture &; // 重置 texture 为未初始化状态

  // 与另一个纹理交换数据
  auto        exch(BaseTexture &tex) -> BaseTexture &;
  static void exch(BaseTexture &tex1, BaseTexture &tex2) {
    tex1.exch(tex2);
  }

  // 是否已经初始化
  auto ready() const -> bool {
    return pixels != null;
  }

  // 使用运算符访问不进行安全检查
  INLINE auto operator[](i32 y) const -> const T * {
    return &pixels[y * pitch];
  }
  INLINE auto operator[](i32 y) -> T * {
    return &pixels[y * pitch];
  }
  INLINE auto operator()(i32 x, i32 y) const -> const T & {
    return pixels[y * pitch + x];
  }
  INLINE auto operator()(i32 x, i32 y) -> T & {
    return pixels[y * pitch + x];
  }
  // 不进行安全检查
  INLINE auto pix(ssize_t i) const -> const T & {
    return pixels[i];
  }
  INLINE auto pix(ssize_t i) -> T & {
    return pixels[i];
  }
  INLINE auto pix(i32 x, i32 y) const -> const T & {
    return pixels[y * pitch + x];
  }
  INLINE auto pix(i32 x, i32 y) -> T & {
    return pixels[y * pitch + x];
  }
  // 进行安全检查
  FLATTEN auto at(ssize_t i) const -> const T & {
    i = cpp::clamp(i, (ssize_t)0, (ssize_t)size - 1);
    return pixels[i];
  }
  FLATTEN auto at(ssize_t i) -> T & {
    i = cpp::clamp(i, (ssize_t)0, (ssize_t)size - 1);
    return pixels[i];
  }
  FLATTEN auto at(i32 x, i32 y) const -> const T & {
    x = cpp::clamp(x, 0, (i32)width - 1);
    y = cpp::clamp(y, 0, (i32)height - 1);
    return pixels[y * pitch + x];
  }
  FLATTEN auto at(i32 x, i32 y) -> T & {
    x = cpp::clamp(x, 0, (i32)width - 1);
    y = cpp::clamp(y, 0, (i32)height - 1);
    return pixels[y * pitch + x];
  }
  FLATTEN auto lerpat(f32 x, f32 y) const -> T {
    x            = cpp::clamp(x, 0.f, (f32)width - 1);
    y            = cpp::clamp(y, 0.f, (f32)height - 1);
    const i32 x1 = (i32)x;
    const i32 y1 = (i32)y;
    const i32 x2 = cpp::min(x1 + 1, (i32)width - 1);
    const i32 y2 = cpp::min(y1 + 1, (i32)height - 1);
    const f32 dx = x - x1;
    const f32 dy = y - y1;
    const T  &p1 = (*this)(x1, y1);
    const T  &p2 = (*this)(x2, y1);
    const T  &p3 = (*this)(x1, y2);
    const T  &p4 = (*this)(x2, y2);
    return p1 * (1 - dx) * (1 - dy) + p2 * dx * (1 - dy) + p3 * (1 - dx) * dy + p4 * dx * dy;
  }

  // 获取与 texture 一样大的 rect
  FLATTEN auto size_rect() const -> Rect {
    return {0, 0, (i32)width - 1, (i32)height - 1};
  }

  // 拷贝到另一个 texture，从另一个 texture 拷贝
  auto copy() -> BaseTexture *;
  auto copy_u8() -> TextureB *;
  auto copy_f32() -> TextureF *;
  template <typename T2>
  auto copy_to(BaseTexture<T2> &d) const -> bool {
    return d.copy_from(*this);
  }
  template <typename T2>
  auto copy_from(const BaseTexture<T2> &d) -> bool;

  auto clear() -> BaseTexture &; // 重置为透明，禁用透明则为黑色

  // 获取、设置值
  INLINE auto get(i32 x, i32 y) -> T &;
  INLINE auto get(i32 x, i32 y) const -> const T &;
  INLINE auto get(i32 x, i32 y, T &p) -> BaseTexture &;
  INLINE auto get(i32 x, i32 y, T &p) const -> const BaseTexture &;
  INLINE auto set(i32 x, i32 y, const T &p) -> BaseTexture &;
  INLINE auto set(i32 x, i32 y, u32 c) -> BaseTexture &;
  INLINE auto set(i32 x, i32 y, byte r, byte g, byte b) -> BaseTexture &;
  INLINE auto set(i32 x, i32 y, byte r, byte g, byte b, byte a) -> BaseTexture &;
  INLINE auto set(i32 x, i32 y, f32 r, f32 g, f32 b) -> BaseTexture &;
  INLINE auto set(i32 x, i32 y, f32 r, f32 g, f32 b, f32 a) -> BaseTexture &;

  auto gaussian_blur(i32 size = 7, f32 sigma = 1) -> BaseTexture &; // 高斯模糊
  auto blur() -> BaseTexture & {
    return gaussian_blur();
  }

  auto fft() -> BaseTexture &;     // 对图像进行快速傅里叶变换
  auto fft_row() -> BaseTexture &; // 按行对图像进行快速傅里叶变换
  auto fft_col() -> BaseTexture &; // 按列对图像进行快速傅里叶变换
  auto ift() -> BaseTexture &;     // 对图像进行快速逆傅里叶变换
  auto ift_row() -> BaseTexture &; // 按行对图像进行快速逆傅里叶变换
  auto ift_col() -> BaseTexture &; // 按列对图像进行快速逆傅里叶变换

  auto fft_resize(f32 s) -> BaseTexture &;
  auto fft_resize(u32 w, u32 h) -> BaseTexture &;
  auto fft_resize_copy(f32 s) -> BaseTexture *;
  auto fft_resize_copy(u32 w, u32 h) -> BaseTexture *;

  auto lerp_resize(f32 s) -> BaseTexture &;
  auto lerp_resize(u32 w, u32 h) -> BaseTexture &;
  auto lerp_resize_copy(f32 s) -> BaseTexture *;
  auto lerp_resize_copy(u32 w, u32 h) -> BaseTexture *;

  //% 缩放 resize_copy会创建新对象 resize不会创建新对象
  auto resize(float s) -> BaseTexture &;           // 缩放到 s 倍
  auto resize(u32 w, u32 h) -> BaseTexture &;      // 缩放到指定大小
  auto resize_copy(float s) -> BaseTexture *;      // 缩放到 s 倍
  auto resize_copy(u32 w, u32 h) -> BaseTexture *; // 缩放到指定大小

  //% 纹理内置的简单绘图函数

  // 绘制线条
  auto line(const Point2I &p1, const Point2I &p2, const T &color) -> BaseTexture &;
  auto line_mix(const Point2I &p1, const Point2I &p2, const T &color) -> BaseTexture &;
  auto line(i32 x1, i32 y1, i32 x2, i32 y2, const T &color) -> BaseTexture &;
  auto line_mix(i32 x1, i32 y1, i32 x2, i32 y2, const T &color) -> BaseTexture &;
  auto trangle(const Point2I &p1, const Point2I &p2, const Point2I &p3, const T &color)
      -> BaseTexture &;
  auto polygon() -> BaseTexture &;

  // 填充区域
  auto fill(const T &color) -> BaseTexture &;
  auto fill(RectI rect, const T &color) -> BaseTexture &;
  auto fill_mix(RectI rect, const T &color) -> BaseTexture &;
  auto fill(const T &(*cb)(i32 x, i32 y)) -> BaseTexture &;
  auto fill_trangle(const Point2I &p1, const Point2I &p2, const Point2I &p3, const T &color)
      -> BaseTexture &;

  // 绘制图片
  template <typename T2>
  auto paste_from(const BaseTexture<T2> &tex, i32 x, i32 y) -> BaseTexture &;
  template <typename T2>
  auto paste_from_mix(const BaseTexture<T2> &tex, i32 x, i32 y) -> BaseTexture &;
  template <typename T2>
  auto paste_from_opaque(const BaseTexture<T2> &tex, i32 x, i32 y) -> BaseTexture &;
  template <typename T2>
  auto paste_to(BaseTexture<T2> &tex, i32 x, i32 y) -> BaseTexture &;
  template <typename T2>
  auto paste_to(BaseTexture<T2> &tex, i32 x, i32 y) const -> const BaseTexture &;
  template <typename T2>
  auto paste_to_mix(BaseTexture<T2> &tex, i32 x, i32 y) -> BaseTexture &;
  template <typename T2>
  auto paste_to_mix(BaseTexture<T2> &tex, i32 x, i32 y) const -> const BaseTexture &;
  template <typename T2>
  auto paste_to_opaque(BaseTexture<T2> &tex, i32 x, i32 y) -> BaseTexture &;
  template <typename T2>
  auto paste_to_opaque(BaseTexture<T2> &tex, i32 x, i32 y) const -> const BaseTexture &;

  // 转换颜色
  INLINE auto replace(const T &src, const T &dst) -> BaseTexture &;
  INLINE auto transform(void (*cb)(T &pix)) -> BaseTexture &;
  INLINE auto transform(void (*cb)(BaseTexture &t, T &pix)) -> BaseTexture &;
  INLINE auto transform(void (*cb)(T &pix, i32 x, i32 y)) -> BaseTexture &;
  INLINE auto transform(void (*cb)(BaseTexture &t, T &pix, i32 x, i32 y)) -> BaseTexture &;

  template <typename U>
  auto add_clamp(const BaseTexture<U> &tex) -> BaseTexture &;
  auto clamp() -> BaseTexture &;                          // 将颜色值限制在 0 到 1 之间
  auto copy_clamp() -> BaseTexture *;                     // 将颜色值限制在 0 到 1 之间
  auto clamp(T::TYPE low, T::TYPE high) -> BaseTexture &; // 将颜色值限制在 low 到 high 之间
  auto glow() -> BaseTexture &;                           // 亮度超过 1 的像素的辉光效果

  auto add(f32 v) -> BaseTexture & {
    for (usize i = 0; i < size; i++) {
      pixels[i] = (pixels[i].to_float() + PixelF(v, v, v, 0)).clamp();
    }
    return *this;
  }
  auto minus_clamp(f32 v) -> BaseTexture & {
    for (usize i = 0; i < size; i++) {
      pixels[i] = (pixels[i].to_float() - PixelF(v, v, v, 0)).clamp();
    }
    return *this;
  }

  auto mulby(f32 s) -> BaseTexture & {
    for (usize i = 0; i < size; i++) {
      pixels[i] *= s;
    }
    return *this;
  }
  auto divby(f32 s) -> BaseTexture & {
    for (usize i = 0; i < size; i++) {
      pixels[i] /= s;
    }
    return *this;
  }
  auto mulby_clamp(f32 s) -> BaseTexture & {
    for (usize i = 0; i < size; i++) {
      pixels[i] = (pixels[i].to_float() * s).clamp();
    }
    return *this;
  }
  auto divby_clamp(f32 s) -> BaseTexture & {
    for (usize i = 0; i < size; i++) {
      pixels[i] = (pixels[i].to_float() / s).clamp();
    }
    return *this;
  }
};

} // namespace pl2d
