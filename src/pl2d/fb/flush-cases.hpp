#pragma once
#include "private.hpp"

namespace pl2d {

#define RUN                                                                                        \
  do {                                                                                             \
    switch (pixfmt) {                                                                              \
      CASE(RGB565);                                                                                \
      CASE(BGR565);                                                                                \
      CASE(RGB);                                                                                   \
      CASE(BGR);                                                                                   \
      CASE(RGBA);                                                                                  \
      CASE(BGRA);                                                                                  \
      CASE(ARGB);                                                                                  \
      CASE(ABGR);                                                                                  \
    default: ERROR(1, "不支持的像素格式");                                                         \
    }                                                                                              \
    return 0;                                                                                      \
  } while (0)

#define CASE(_name_)                                                                               \
  case PixFmt::_name_: framebuffer::fb_flush<PixFmt::_name_>(*this, tex, rect); break
auto FrameBuffer::flush(const pl2d::TextureB &tex, const pl2d::Rect &rect) -> int {
  RUN;
}
auto FrameBuffer::flush(const pl2d::TextureF &tex, const pl2d::Rect &rect) -> int {
  RUN;
}
#undef CASE

#define CASE(_name_)                                                                               \
  case PixFmt::_name_: framebuffer::fb_copy_to<PixFmt::_name_>(*this, tex, rect); break
auto FrameBuffer::copy_to(pl2d::TextureB &tex, const pl2d::Rect &rect) const -> int {
  RUN;
}
auto FrameBuffer::copy_to(pl2d::TextureF &tex, const pl2d::Rect &rect) const -> int {
  RUN;
}
#undef CASE

#undef RUN

auto FrameBuffer::flush(const pl2d::TextureB &tex) -> int {
  pl2d::Rect rect;
  rect.x2 = cpp::min(width, tex.width) - 1;
  rect.y2 = cpp::min(height, tex.height) - 1;
  return flush(tex, rect);
}
auto FrameBuffer::flush(const pl2d::TextureF &tex) -> int {
  pl2d::Rect rect;
  rect.x2 = cpp::min(width, tex.width) - 1;
  rect.y2 = cpp::min(height, tex.height) - 1;
  return flush(tex, rect);
}
auto FrameBuffer::copy_to(pl2d::TextureB &tex) const -> int {
  pl2d::Rect rect;
  rect.x2 = cpp::min(width, tex.width) - 1;
  rect.y2 = cpp::min(height, tex.height) - 1;
  return copy_to(tex, rect);
}
auto FrameBuffer::copy_to(pl2d::TextureF &tex) const -> int {
  pl2d::Rect rect;
  rect.x2 = cpp::min(width, tex.width) - 1;
  rect.y2 = cpp::min(height, tex.height) - 1;
  return copy_to(tex, rect);
}

} // namespace pl2d
