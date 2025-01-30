
#include <pl2d.hpp>

namespace pl2d {

template <typename T>
auto BaseTexture<T>::glow() -> BaseTexture & {
  val tex = this->copy();
  for (usize i = 0; i < size; i++) {
    pixels[i].r = pixels[i].r > 1 ? pixels[i].r : 0;
    pixels[i].g = pixels[i].g > 1 ? pixels[i].g : 0;
    pixels[i].b = pixels[i].b > 1 ? pixels[i].b : 0;
  }
  this->gaussian_blur(11, 5).add_clamp(*tex);
  delete tex;
  return *this;
}

BaseTextureInstantiation

} // namespace pl2d
