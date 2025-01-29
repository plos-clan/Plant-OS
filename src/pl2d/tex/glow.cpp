
#include <pl2d.hpp>

namespace pl2d {

template <typename T>
auto BaseTexture<T>::glow() -> BaseTexture & {
  val tex = this->copy();
  this->minus_clamp(1).mulby(4).gaussian_blur(11, 5).add_clamp(*tex);
  delete tex;
  return *this;
}

BaseTextureInstantiation

} // namespace pl2d
