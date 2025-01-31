
#include <pl2d.hpp>

namespace pl2d {

template <typename T>
BaseTexture<T>::BaseTexture(u32 width, u32 height)
    : width(width), height(height), pitch(width), size(width * height) {
  own_pixels   = true;
  alloced_size = size;
  pixels       = (T *)malloc(size * sizeof(T));
  free_pixels  = free;
  assert(pixels != null);
}

template <typename T>
BaseTexture<T>::BaseTexture(u32 width, u32 height, u32 pitch)
    : width(width), height(height), pitch(pitch), size(pitch * height) {
  own_pixels   = true;
  alloced_size = size;
  pixels       = (T *)malloc(size * sizeof(T));
  free_pixels  = free;
  assert(pixels != null);
}

template <typename T>
BaseTexture<T>::BaseTexture(T *pixels, u32 width, u32 height)
    : width(width), height(height), pitch(width), size(width * height) {
  assert(pixels != null);
  this->pixels = pixels;
}

template <typename T>
BaseTexture<T>::BaseTexture(T *pixels, u32 width, u32 height, u32 pitch)
    : width(width), height(height), pitch(pitch), size(pitch * height) {
  assert(pixels != null);
  this->pixels = pixels;
}

template <typename T>
BaseTexture<T>::~BaseTexture() noexcept {
  if (own_pixels) {
    if (refcnted_pixels) {
      cpp::rc::unref(pixels);
    } else {
      free(pixels);
    }
  }
}

template <typename T>
BaseTexture<T>::BaseTexture(BaseTexture &&tex) noexcept {
  pixels           = tex.pixels;
  own_pixels       = tex.own_pixels;
  width            = tex.width;
  height           = tex.height;
  pitch            = tex.pitch;
  size             = tex.size;
  alloced_size     = tex.alloced_size;
  tex.pixels       = null;
  tex.own_pixels   = false;
  tex.width        = 0;
  tex.height       = 0;
  tex.pitch        = 0;
  tex.size         = 0;
  tex.alloced_size = 0;
}

template <typename T>
auto BaseTexture<T>::operator=(BaseTexture &&tex) noexcept -> BaseTexture & {
  if (this == &tex) return *this;
  this->~BaseTexture();
  pixels           = tex.pixels;
  own_pixels       = tex.own_pixels;
  width            = tex.width;
  height           = tex.height;
  pitch            = tex.pitch;
  size             = tex.size;
  alloced_size     = tex.alloced_size;
  tex.pixels       = null;
  tex.own_pixels   = false;
  tex.width        = 0;
  tex.height       = 0;
  tex.pitch        = 0;
  tex.size         = 0;
  tex.alloced_size = 0;
  return *this;
}

template <typename T>
auto BaseTexture<T>::reset() -> BaseTexture & {
  this->~BaseTexture();
  *this = {};
  return *this;
}

template <typename T>
auto BaseTexture<T>::exch(BaseTexture &tex) -> BaseTexture & {
  cpp::exch(pixels, tex.pixels);
  cpp::exch(own_pixels, tex.own_pixels);
  cpp::exch(width, tex.width);
  cpp::exch(height, tex.height);
  cpp::exch(pitch, tex.pitch);
  cpp::exch(size, tex.size);
  cpp::exch(alloced_size, tex.alloced_size);
  return *this;
}

template <typename T>
auto BaseTexture<T>::clear() -> BaseTexture & {
  memset(pixels, 0, size * sizeof(T));
  return *this;
}

BaseTextureInstantiation

} // namespace pl2d
