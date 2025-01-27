#pragma once
#include "private.hpp"

namespace pl2d::framebuffer {

template <>
finline void fb_flush_pix<PixFmt::RGB>(FrameBuffer &fb, const pl2d::TextureB &tex, u32 x, u32 y) {
  byte *_rest fb_p  = &fb.pix8[0][y * fb.pitch + x * fb.padding];
  auto *_rest tex_p = &tex.pixels[y * tex.pitch + x];
  fb_p[0]           = tex_p->r;
  fb_p[1]           = tex_p->g;
  fb_p[2]           = tex_p->b;
}
template <>
finline void fb_flush_pix<PixFmt::RGB>(FrameBuffer &fb, const pl2d::TextureF &tex, u32 x, u32 y) {
  byte *_rest fb_p  = &fb.pix8[0][y * fb.pitch + x * fb.padding];
  auto *_rest tex_p = &tex.pixels[y * tex.pitch + x];
  fb_p[0]           = tex_p->r * 255.f;
  fb_p[1]           = tex_p->g * 255.f;
  fb_p[2]           = tex_p->b * 255.f;
}
template <>
finline void fb_copy_to_pix<PixFmt::RGB>(const FrameBuffer &fb, pl2d::TextureB &tex, u32 x, u32 y) {
  byte *_rest fb_p  = &fb.pix8[0][y * fb.pitch + x * fb.padding];
  auto *_rest tex_p = &tex.pixels[y * tex.pitch + x];
  tex_p->r          = fb_p[0];
  tex_p->g          = fb_p[1];
  tex_p->b          = fb_p[2];
  tex_p->a          = 255;
}
template <>
finline void fb_copy_to_pix<PixFmt::RGB>(const FrameBuffer &fb, pl2d::TextureF &tex, u32 x, u32 y) {
  byte *_rest fb_p  = &fb.pix8[0][y * fb.pitch + x * fb.padding];
  auto *_rest tex_p = &tex.pixels[y * tex.pitch + x];
  tex_p->r          = fb_p[0] / 255.f;
  tex_p->g          = fb_p[1] / 255.f;
  tex_p->b          = fb_p[2] / 255.f;
  tex_p->a          = 1;
}

template <>
finline void fb_flush_pix<PixFmt::RGBA>(FrameBuffer &fb, const pl2d::TextureB &tex, u32 x, u32 y) {
  byte *_rest fb_p  = &fb.pix8[0][y * fb.pitch + x * fb.padding];
  auto *_rest tex_p = &tex.pixels[y * tex.pitch + x];
  fb_p[0]           = tex_p->r;
  fb_p[1]           = tex_p->g;
  fb_p[2]           = tex_p->b;
  fb_p[3]           = tex_p->a;
}
template <>
finline void fb_flush_pix<PixFmt::RGBA>(FrameBuffer &fb, const pl2d::TextureF &tex, u32 x, u32 y) {
  byte *_rest fb_p  = &fb.pix8[0][y * fb.pitch + x * fb.padding];
  auto *_rest tex_p = &tex.pixels[y * tex.pitch + x];
  fb_p[0]           = tex_p->r * 255.f;
  fb_p[1]           = tex_p->g * 255.f;
  fb_p[2]           = tex_p->b * 255.f;
  fb_p[3]           = tex_p->a * 255.f;
}
template <>
finline void fb_copy_to_pix<PixFmt::RGBA>(const FrameBuffer &fb, pl2d::TextureB &tex, u32 x,
                                          u32 y) {
  byte *_rest fb_p  = &fb.pix8[0][y * fb.pitch + x * fb.padding];
  auto *_rest tex_p = &tex.pixels[y * tex.pitch + x];
  tex_p->r          = fb_p[0];
  tex_p->g          = fb_p[1];
  tex_p->b          = fb_p[2];
  tex_p->a          = fb_p[3];
}
template <>
finline void fb_copy_to_pix<PixFmt::RGBA>(const FrameBuffer &fb, pl2d::TextureF &tex, u32 x,
                                          u32 y) {
  byte *_rest fb_p  = &fb.pix8[0][y * fb.pitch + x * fb.padding];
  auto *_rest tex_p = &tex.pixels[y * tex.pitch + x];
  tex_p->r          = fb_p[0] / 255.f;
  tex_p->g          = fb_p[1] / 255.f;
  tex_p->b          = fb_p[2] / 255.f;
  tex_p->a          = fb_p[3] / 255.f;
}

template <>
finline void fb_flush_pix<PixFmt::ARGB>(FrameBuffer &fb, const pl2d::TextureB &tex, u32 x, u32 y) {
  byte *_rest fb_p  = &fb.pix8[0][y * fb.pitch + x * fb.padding];
  auto *_rest tex_p = &tex.pixels[y * tex.pitch + x];
  fb_p[0]           = tex_p->a;
  fb_p[1]           = tex_p->r;
  fb_p[2]           = tex_p->g;
  fb_p[3]           = tex_p->b;
}
template <>
finline void fb_flush_pix<PixFmt::ARGB>(FrameBuffer &fb, const pl2d::TextureF &tex, u32 x, u32 y) {
  byte *_rest fb_p  = &fb.pix8[0][y * fb.pitch + x * fb.padding];
  auto *_rest tex_p = &tex.pixels[y * tex.pitch + x];
  fb_p[0]           = tex_p->a * 255.f;
  fb_p[1]           = tex_p->r * 255.f;
  fb_p[2]           = tex_p->g * 255.f;
  fb_p[3]           = tex_p->b * 255.f;
}
template <>
finline void fb_copy_to_pix<PixFmt::ARGB>(const FrameBuffer &fb, pl2d::TextureB &tex, u32 x,
                                          u32 y) {
  byte *_rest fb_p  = &fb.pix8[0][y * fb.pitch + x * fb.padding];
  auto *_rest tex_p = &tex.pixels[y * tex.pitch + x];
  tex_p->a          = fb_p[0];
  tex_p->r          = fb_p[1];
  tex_p->g          = fb_p[2];
  tex_p->b          = fb_p[3];
}
template <>
finline void fb_copy_to_pix<PixFmt::ARGB>(const FrameBuffer &fb, pl2d::TextureF &tex, u32 x,
                                          u32 y) {
  byte *_rest fb_p  = &fb.pix8[0][y * fb.pitch + x * fb.padding];
  auto *_rest tex_p = &tex.pixels[y * tex.pitch + x];
  tex_p->a          = fb_p[0] / 255.f;
  tex_p->r          = fb_p[1] / 255.f;
  tex_p->g          = fb_p[2] / 255.f;
  tex_p->b          = fb_p[3] / 255.f;
}

} // namespace pl2d::framebuffer
