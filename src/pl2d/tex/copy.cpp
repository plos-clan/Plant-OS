
#include <pl2d.hpp>

namespace pl2d {

template <typename T>
auto BaseTexture<T>::copy() -> BaseTexture * {
  auto *d = new BaseTexture(width, height, pitch);
  if (d == null) return null;
  d->copy_from(*this);
  return d;
}

template <typename T>
auto BaseTexture<T>::copy_u8() -> TextureB * {
  auto *d = new TextureB(width, height, pitch);
  if (d == null) return null;
  d->copy_from(*this);
  return d;
}

template <typename T>
auto BaseTexture<T>::copy_f32() -> TextureF * {
  auto *d = new TextureF(width, height, pitch);
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
    for (u32 y = 0; y < height; y++) {
      for (u32 x = 0; x < width; x++) {
        (*this)(x, y) = d(x, y);
      }
    }
  }
  return true;
}

template dlexport auto BaseTexture<PixelB>::copy_from(const BaseTexture<PixelB> &d) -> bool;
template dlexport auto BaseTexture<PixelB>::copy_from(const BaseTexture<PixelS> &d) -> bool;
template dlexport auto BaseTexture<PixelB>::copy_from(const BaseTexture<PixelF> &d) -> bool;
template dlexport auto BaseTexture<PixelB>::copy_from(const BaseTexture<PixelD> &d) -> bool;
template dlexport auto BaseTexture<PixelS>::copy_from(const BaseTexture<PixelB> &d) -> bool;
template dlexport auto BaseTexture<PixelS>::copy_from(const BaseTexture<PixelS> &d) -> bool;
template dlexport auto BaseTexture<PixelS>::copy_from(const BaseTexture<PixelF> &d) -> bool;
template dlexport auto BaseTexture<PixelS>::copy_from(const BaseTexture<PixelD> &d) -> bool;
template dlexport auto BaseTexture<PixelF>::copy_from(const BaseTexture<PixelB> &d) -> bool;
template dlexport auto BaseTexture<PixelF>::copy_from(const BaseTexture<PixelS> &d) -> bool;
template dlexport auto BaseTexture<PixelF>::copy_from(const BaseTexture<PixelF> &d) -> bool;
template dlexport auto BaseTexture<PixelF>::copy_from(const BaseTexture<PixelD> &d) -> bool;
template dlexport auto BaseTexture<PixelD>::copy_from(const BaseTexture<PixelB> &d) -> bool;
template dlexport auto BaseTexture<PixelD>::copy_from(const BaseTexture<PixelS> &d) -> bool;
template dlexport auto BaseTexture<PixelD>::copy_from(const BaseTexture<PixelF> &d) -> bool;
template dlexport auto BaseTexture<PixelD>::copy_from(const BaseTexture<PixelD> &d) -> bool;

BaseTextureInstantiation

} // namespace pl2d
