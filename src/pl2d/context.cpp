#include <pl2d.hpp>

namespace pl2d {

//* ----------------------------------------------------------------------------------------------------
//; Context
//* ----------------------------------------------------------------------------------------------------

Context::Context(TextureB *tex) : tex(tex) {
  rect.x1 = 0;
  rect.y1 = 0;
  rect.x2 = tex->width - 1;
  rect.y2 = tex->height - 1;
}

// Context::Context(u32 width, u32 height) : own_tex(true) {
//   tex     = new Texture();
//   rect.x1 = 0;
//   rect.y1 = 0;
//   rect.x2 = width - 1;
//   rect.y2 = height - 1;
// }

} // namespace pl2d
