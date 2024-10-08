
#include <pl2d.hpp>

namespace pl2d {

template <typename T>
auto BaseTexture<T>::copy() -> BaseTexture<T> * {
  auto *d = new BaseTexture<T>(width, height, pitch);
  if (d == null) return null;
  d->copy_from(*this);
  return d;
}

template <typename T>
template <typename T2>
auto BaseTexture<T>::copy_from(const BaseTexture<T2> &d) -> bool {
  if (d.width != width || d.height != height) return false;
  if constexpr (std::is_same_v<T, T2>) {
    if (&d == this) return true;
    if (d.pitch == pitch) {
      memcpy(pixels, d.pixels, size * sizeof(T));
      return true;
    }
    for (u32 i = 0; i < height; i++) {
      memcpy(&pixels[i * pitch], &d.pixels[i * d.pitch], width * sizeof(T));
    }
  } else {
    for (const auto [x, y] : d.size_rect()) {
      (*this)(x, y) = d(x, y);
    }
  }
  return true;
}

template auto BaseTexture<PixelB>::copy_from(const BaseTexture<PixelB> &d) -> bool;
template auto BaseTexture<PixelB>::copy_from(const BaseTexture<PixelS> &d) -> bool;
template auto BaseTexture<PixelB>::copy_from(const BaseTexture<PixelF> &d) -> bool;
template auto BaseTexture<PixelB>::copy_from(const BaseTexture<PixelD> &d) -> bool;
template auto BaseTexture<PixelS>::copy_from(const BaseTexture<PixelB> &d) -> bool;
template auto BaseTexture<PixelS>::copy_from(const BaseTexture<PixelS> &d) -> bool;
template auto BaseTexture<PixelS>::copy_from(const BaseTexture<PixelF> &d) -> bool;
template auto BaseTexture<PixelS>::copy_from(const BaseTexture<PixelD> &d) -> bool;
template auto BaseTexture<PixelF>::copy_from(const BaseTexture<PixelB> &d) -> bool;
template auto BaseTexture<PixelF>::copy_from(const BaseTexture<PixelS> &d) -> bool;
template auto BaseTexture<PixelF>::copy_from(const BaseTexture<PixelF> &d) -> bool;
template auto BaseTexture<PixelF>::copy_from(const BaseTexture<PixelD> &d) -> bool;
template auto BaseTexture<PixelD>::copy_from(const BaseTexture<PixelB> &d) -> bool;
template auto BaseTexture<PixelD>::copy_from(const BaseTexture<PixelS> &d) -> bool;
template auto BaseTexture<PixelD>::copy_from(const BaseTexture<PixelF> &d) -> bool;
template auto BaseTexture<PixelD>::copy_from(const BaseTexture<PixelD> &d) -> bool;

BaseTextureInstantiation

} // namespace pl2d
