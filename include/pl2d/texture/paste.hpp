#pragma once
#include "texture.hpp"

namespace pl2d {

template <typename T>
template <typename T2>
auto BaseTexture<T>::paste_from(const BaseTexture<T2> &tex, i32 dx, i32 dy) -> BaseTexture & {
  Rect rect = {(i32)tex.width, (i32)tex.height};
  rect.trunc(-dx, -dy, (i32)width - dx - 1, (i32)height - dy - 1);
#if USE_ITERATOR
  for (const auto [x, y] : rect) {
    (*this)(x + dx, y + dy) = tex(x, y);
  }
#else
  for (i32 y = rect.y1; y <= rect.y2; y++) {
    for (i32 x = rect.x1; x <= rect.x2; x++) {
      (*this)(x + dx, y + dy) = tex(x, y);
    }
  }
#endif
  return *this;
}

template <typename T>
template <typename T2>
auto BaseTexture<T>::paste_from_mix(const BaseTexture<T2> &tex, i32 dx, i32 dy) -> BaseTexture & {
  Rect rect = {(i32)tex.width, (i32)tex.height};
  rect.trunc(-dx, -dy, (i32)width - dx - 1, (i32)height - dy - 1);
#if USE_ITERATOR
  for (const auto [x, y] : rect) {
    (*this)(x + dx, y + dy).mix(tex(x, y));
  }
#else
  for (i32 y = rect.y1; y <= rect.y2; y++) {
    for (i32 x = rect.x1; x <= rect.x2; x++) {
      (*this)(x + dx, y + dy).mix(tex(x, y));
    }
  }
#endif
  return *this;
}

template <typename T>
template <typename T2>
auto BaseTexture<T>::paste_from_opaque(const BaseTexture<T2> &tex, i32 dx, i32 dy)
    -> BaseTexture & {
  Rect rect = {(i32)tex.width, (i32)tex.height};
  rect.trunc(-dx, -dy, (i32)width - dx - 1, (i32)height - dy - 1);
#if USE_ITERATOR
  for (const auto [x, y] : rect) {
    (*this)(x + dx, y + dy).mix_opaque(tex(x, y));
  }
#else
  for (i32 y = rect.y1; y <= rect.y2; y++) {
    for (i32 x = rect.x1; x <= rect.x2; x++) {
      (*this)(x + dx, y + dy).mix_opaque(tex(x, y));
    }
  }
#endif
  return *this;
}

template <typename T>
template <typename T2>
auto BaseTexture<T>::paste_to(BaseTexture<T2> &tex, i32 x, i32 y) -> BaseTexture & {
  tex.paste_from(*this, x, y);
  return *this;
}
template <typename T>
template <typename T2>
auto BaseTexture<T>::paste_to(BaseTexture<T2> &tex, i32 x, i32 y) const -> const BaseTexture & {
  tex.paste_from(*this, x, y);
  return *this;
}
template <typename T>
template <typename T2>
auto BaseTexture<T>::paste_to_mix(BaseTexture<T2> &tex, i32 x, i32 y) -> BaseTexture & {
  tex.paste_from_mix(*this, x, y);
  return *this;
}
template <typename T>
template <typename T2>
auto BaseTexture<T>::paste_to_mix(BaseTexture<T2> &tex, i32 x, i32 y) const -> const BaseTexture & {
  tex.paste_from_mix(*this, x, y);
  return *this;
}
template <typename T>
template <typename T2>
auto BaseTexture<T>::paste_to_opaque(BaseTexture<T2> &tex, i32 x, i32 y) -> BaseTexture & {
  tex.paste_from_opaque(*this, x, y);
  return *this;
}
template <typename T>
template <typename T2>
auto BaseTexture<T>::paste_to_opaque(BaseTexture<T2> &tex, i32 x, i32 y) const
    -> const BaseTexture & {
  tex.paste_from_opaque(*this, x, y);
  return *this;
}

} // namespace pl2d
