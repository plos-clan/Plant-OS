
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
auto BaseTexture<T>::fft_resize_copy(u32 w, u32 h) -> BaseTexture * {}

BaseTextureInstantiation

} // namespace pl2d
