#pragma once
#include "texture.hpp"

namespace pl2d {

#if USE_ITERATOR

template <typename T>
auto BaseTexture<T>::replace(const T &src, const T dst) -> BaseTexture & {
  for (const auto [x, y] : size_rect()) {
    if ((*this)(x, y) == src) (*this)(x, y) = dst;
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::transform(void (*cb)(T &pix)) -> BaseTexture & {
  for (const auto [x, y] : size_rect()) {
    cb((*this)(x, y));
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::transform(void (*cb)(BaseTexture &t, T &pix)) -> BaseTexture & {
  for (const auto [x, y] : size_rect()) {
    cb(*this, (*this)(x, y));
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::transform(void (*cb)(T &pix, i32 x, i32 y)) -> BaseTexture & {
  for (const auto [x, y] : size_rect()) {
    cb((*this)(x, y), x, y);
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::transform(void (*cb)(BaseTexture &t, T &pix, i32 x, i32 y)) -> BaseTexture & {
  for (const auto [x, y] : size_rect()) {
    cb(*this, (*this)(x, y), x, y);
  }
  return *this;
}

#else

template <typename T>
auto BaseTexture<T>::replace(const T &src, const T dst) -> BaseTexture & {
  for (i32 y = 0; y < height; y++) {
    for (i32 x = 0; x < width; x++) {
      if ((*this)(x, y) == src) (*this)(x, y) = dst;
    }
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::transform(void (*cb)(T &pix)) -> BaseTexture & {
  for (i32 y = 0; y < height; y++) {
    for (i32 x = 0; x < width; x++) {
      cb((*this)(x, y));
    }
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::transform(void (*cb)(BaseTexture &t, T &pix)) -> BaseTexture & {
  for (i32 y = 0; y < height; y++) {
    for (i32 x = 0; x < width; x++) {
      cb(*this, (*this)(x, y));
    }
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::transform(void (*cb)(T &pix, i32 x, i32 y)) -> BaseTexture & {
  for (i32 y = 0; y < height; y++) {
    for (i32 x = 0; x < width; x++) {
      cb((*this)(x, y), x, y);
    }
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::transform(void (*cb)(BaseTexture &t, T &pix, i32 x, i32 y)) -> BaseTexture & {
  for (i32 y = 0; y < height; y++) {
    for (i32 x = 0; x < width; x++) {
      cb(*this, (*this)(x, y), x, y);
    }
  }
  return *this;
}

#endif

} // namespace pl2d
