
#include <pl2d.hpp>

namespace pl2d {

template <typename T>
auto BaseTexture<T>::fft_resize(f32 s) -> BaseTexture & {
  u32 w = width * s, h = height * s;
  return fft_resize(w, h);
}

template <typename T>
auto BaseTexture<T>::fft_resize_copy(f32 s) -> BaseTexture * {
  u32 w = width * s, h = height * s;
  return fft_resize_copy(w, h);
}

template <>
auto BaseTexture<PixelF>::fft_resize(u32 w, u32 h) -> BaseTexture & {
  BaseTexture tmp(width, height);
  copy_to(tmp);
  tmp.fft();
  BaseTexture tmp2(w, h);
  tmp.paste_to(tmp2, 0, 0);
  tmp2.ift();
  *this = {w, h};
  tmp2.copy_to(*this);
  return *this;
}

template <>
auto BaseTexture<PixelF>::fft_resize_copy(u32 w, u32 h) -> BaseTexture * {
  BaseTexture tmp(width, height);
  copy_to(tmp);
  tmp.fft();
  auto *tmp2 = new BaseTexture(w, h);
  tmp.paste_to(*tmp2, 0, 0);
  tmp2->ift();
  return tmp2;
}

template <typename T>
auto BaseTexture<T>::fft_resize(u32 w, u32 h) -> BaseTexture & {
  TextureF tmp(width, height);
  copy_to(tmp);
  tmp.fft();
  auto *tmp2 = new BaseTexture(w, h);
  tmp.paste_to(*tmp2, 0, 0);
  tmp2->ift();
  return *this;
}

template <typename T>
auto BaseTexture<T>::fft_resize_copy(u32 w, u32 h) -> BaseTexture * {
  TextureF tmp(width, height);
  copy_to(tmp);
  tmp.fft();
  auto *tmp2 = new BaseTexture(w, h);
  tmp.paste_to(*tmp2, 0, 0);
  tmp2->ift();
  return tmp2;
}

template <typename T>
auto BaseTexture<T>::lerp_resize(f32 s) -> BaseTexture & {
  if (s == 1) return *this;
  u32 w = width * s, h = height * s;
  return lerp_resize(w, h);
}

template <typename T>
auto BaseTexture<T>::lerp_resize(u32 w, u32 h) -> BaseTexture & {
  if (w == width && h == height) return *this;
  auto *tmp = lerp_resize_copy(w, h);
  exch(*tmp);
  delete tmp;
  return *this;
}

template <typename T>
auto BaseTexture<T>::lerp_resize_copy(f32 s) -> BaseTexture * {
  if (s == 1) return copy();
  u32 w = width * s, h = height * s;
  return lerp_resize_copy(w, h);
}

template <typename T>
auto BaseTexture<T>::lerp_resize_copy(u32 w, u32 h) -> BaseTexture * {
  if (w == width && h == height) return copy();
  auto *tmp = new BaseTexture(w, h);
  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      f32 fx = (f32)x / w * width, fy = (f32)y / h * height;
      tmp->at(x, y) = lerpat(fx, fy);
    }
  }
  return tmp;
}

BaseTextureInstantiation

} // namespace pl2d
