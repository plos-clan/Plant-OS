#pragma once
#include "texture.hpp"

namespace pl2d {

template <typename T>
auto BaseTexture<T>::get(i32 x, i32 y) -> T & {
  cpp::clamp(x, 0, (i32)width - 1);
  cpp::clamp(y, 0, (i32)height - 1);
  return pixels[y * pitch + x];
}
template <typename T>
auto BaseTexture<T>::get(i32 x, i32 y) const -> const T & {
  cpp::clamp(x, 0, (i32)width - 1);
  cpp::clamp(y, 0, (i32)height - 1);
  return pixels[y * pitch + x];
}
template <typename T>
auto BaseTexture<T>::get(i32 x, i32 y, T &p) -> BaseTexture & {
  cpp::clamp(x, 0, (i32)width - 1);
  cpp::clamp(y, 0, (i32)height - 1);
  p = pixels[y * pitch + x];
  return *this;
}
template <typename T>
auto BaseTexture<T>::get(i32 x, i32 y, T &p) const -> const BaseTexture & {
  cpp::clamp(x, 0, (i32)width - 1);
  cpp::clamp(y, 0, (i32)height - 1);
  p = pixels[y * pitch + x];
  return *this;
}
template <typename T>
auto BaseTexture<T>::set(i32 x, i32 y, const T &p) -> BaseTexture & {
  cpp::clamp(x, 0, (i32)width - 1);
  cpp::clamp(y, 0, (i32)height - 1);
  pixels[y * pitch + x] = p;
  return *this;
}

template <typename T>
auto BaseTexture<T>::set(i32 x, i32 y, u32 c) -> BaseTexture & {
  pixels[y * pitch + x] = c;
  return *this;
}

template <typename T>
auto BaseTexture<T>::set(i32 x, i32 y, byte r, byte g, byte b) -> BaseTexture & {
  pixels[y * pitch + x] = PixelB{r, g, b};
  return *this;
}

template <typename T>
auto BaseTexture<T>::set(i32 x, i32 y, byte r, byte g, byte b, byte a) -> BaseTexture & {
  pixels[y * pitch + x] = PixelB{r, g, b, a};
  return *this;
}

template <typename T>
auto BaseTexture<T>::set(i32 x, i32 y, f32 r, f32 g, f32 b) -> BaseTexture & {
  pixels[y * pitch + x] = PixelF{r, g, b};
  return *this;
}

template <typename T>
auto BaseTexture<T>::set(i32 x, i32 y, f32 r, f32 g, f32 b, f32 a) -> BaseTexture & {
  pixels[y * pitch + x] = PixelF{r, g, b, a};
  return *this;
}

} // namespace pl2d
