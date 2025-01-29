
#include <pl2d.hpp>

namespace pl2d {

template <typename T>
auto BaseTexture<T>::clamp() -> BaseTexture & {
  vectorize for (usize i = 0; i < size; i++) {
    pixels[i] = pixels[i].clamp();
  }
  return *this;
}

template <typename T>
auto BaseTexture<T>::clamp(T::TYPE low, T::TYPE high) -> BaseTexture & {
  vectorize for (usize i = 0; i < size; i++) {
    pixels[i] = pixels[i].clamp(low, high);
  }
  return *this;
}

template <typename T>
template <typename U>
auto BaseTexture<T>::add_clamp(const BaseTexture<U> &tex) -> BaseTexture & {
  for (usize y = 0; y < height; y++) {
    vectorize for (usize x = 0; x < width; x++) {
      set(x, y, (get(x, y).to_float() + tex(x, y)).clamp());
    }
  }
  return *this;
}

template dlexport auto BaseTexture<PixelB>::add_clamp(const BaseTexture<PixelB> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelB>::add_clamp(const BaseTexture<PixelS> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelB>::add_clamp(const BaseTexture<PixelF> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelB>::add_clamp(const BaseTexture<PixelD> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelS>::add_clamp(const BaseTexture<PixelB> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelS>::add_clamp(const BaseTexture<PixelS> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelS>::add_clamp(const BaseTexture<PixelF> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelS>::add_clamp(const BaseTexture<PixelD> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelF>::add_clamp(const BaseTexture<PixelB> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelF>::add_clamp(const BaseTexture<PixelS> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelF>::add_clamp(const BaseTexture<PixelF> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelF>::add_clamp(const BaseTexture<PixelD> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelD>::add_clamp(const BaseTexture<PixelB> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelD>::add_clamp(const BaseTexture<PixelS> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelD>::add_clamp(const BaseTexture<PixelF> &) -> BaseTexture &;
template dlexport auto BaseTexture<PixelD>::add_clamp(const BaseTexture<PixelD> &) -> BaseTexture &;

BaseTextureInstantiation

} // namespace pl2d
