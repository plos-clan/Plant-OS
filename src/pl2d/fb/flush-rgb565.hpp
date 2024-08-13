#pragma once
#include "private.hpp"

namespace pl2d::framebuffer {

template <>
void fb_flush_pix<PixFmt::RGB565>(FrameBuffer &fb, const pl2d::TextureB &tex, u32 x, u32 y) {
  auto *fb_p  = &fb.pix16[0][y * fb.pitch + x];
  auto *tex_p = &tex.pixels[y * tex.pitch + x];
  u16   r     = tex_p->r >> 3;
  u16   g     = tex_p->g >> 2;
  u16   b     = tex_p->b >> 3;
  *fb_p       = (b << 11) | (g << 5) | r;
}
template <>
void fb_flush_pix<PixFmt::RGB565>(FrameBuffer &fb, const pl2d::TextureF &tex, u32 x, u32 y) {
  auto *fb_p  = &fb.pix16[0][y * fb.pitch + x];
  auto *tex_p = &tex.pixels[y * tex.pitch + x];
  u16   r     = (u8)(tex_p->r * 255.f) >> 3;
  u16   g     = (u8)(tex_p->g * 255.f) >> 2;
  u16   b     = (u8)(tex_p->b * 255.f) >> 3;
  *fb_p       = (b << 11) | (g << 5) | r;
}
template <>
void fb_copy_to_pix<PixFmt::RGB565>(const FrameBuffer &fb, pl2d::TextureB &tex, u32 x, u32 y) {
  auto *fb_p  = &fb.pix16[0][y * fb.pitch + x];
  auto *tex_p = &tex.pixels[y * tex.pitch + x];
  tex_p->r    = (fb_p[0] & 0b11111) << 3;
  tex_p->g    = ((fb_p[1] >> 5) & 0b111111) << 2;
  tex_p->b    = (fb_p[2] >> 11) << 3;
  tex_p->a    = 255;
}
template <>
void fb_copy_to_pix<PixFmt::RGB565>(const FrameBuffer &fb, pl2d::TextureF &tex, u32 x, u32 y) {
  auto *fb_p  = &fb.pix16[0][y * fb.pitch + x];
  auto *tex_p = &tex.pixels[y * tex.pitch + x];
  tex_p->r    = (fb_p[0] & 0b11111) / 31.f;
  tex_p->g    = ((fb_p[1] >> 5) & 0b111111) / 63.f;
  tex_p->b    = (fb_p[2] >> 11) / 31.f;
  tex_p->a    = 1;
}

template <>
void fb_flush_pix<PixFmt::BGR565>(FrameBuffer &fb, const pl2d::TextureB &tex, u32 x, u32 y) {
  auto *fb_p  = &fb.pix16[0][y * fb.pitch + x];
  auto *tex_p = &tex.pixels[y * tex.pitch + x];
  u16   r     = tex_p->r >> 3;
  u16   g     = tex_p->g >> 2;
  u16   b     = tex_p->b >> 3;
  *fb_p       = (r << 11) | (g << 5) | b;
}
template <>
void fb_flush_pix<PixFmt::BGR565>(FrameBuffer &fb, const pl2d::TextureF &tex, u32 x, u32 y) {
  auto *fb_p  = &fb.pix16[0][y * fb.pitch + x];
  auto *tex_p = &tex.pixels[y * tex.pitch + x];
  u16   r     = (u8)(tex_p->r * 255.f) >> 3;
  u16   g     = (u8)(tex_p->g * 255.f) >> 2;
  u16   b     = (u8)(tex_p->b * 255.f) >> 3;
  *fb_p       = (r << 11) | (g << 5) | b;
}
template <>
void fb_copy_to_pix<PixFmt::BGR565>(const FrameBuffer &fb, pl2d::TextureB &tex, u32 x, u32 y) {
  auto *fb_p  = &fb.pix16[0][y * fb.pitch + x];
  auto *tex_p = &tex.pixels[y * tex.pitch + x];
  tex_p->b    = (fb_p[0] & 0b11111) << 3;
  tex_p->g    = ((fb_p[1] >> 5) & 0b111111) << 2;
  tex_p->r    = (fb_p[2] >> 11) << 3;
  tex_p->a    = 255;
}
template <>
void fb_copy_to_pix<PixFmt::BGR565>(const FrameBuffer &fb, pl2d::TextureF &tex, u32 x, u32 y) {
  auto *fb_p  = &fb.pix16[0][y * fb.pitch + x];
  auto *tex_p = &tex.pixels[y * tex.pitch + x];
  tex_p->b    = (fb_p[0] & 0b11111) / 31.f;
  tex_p->g    = ((fb_p[1] >> 5) & 0b111111) / 63.f;
  tex_p->r    = (fb_p[2] >> 11) / 31.f;
  tex_p->a    = 1;
}

} // namespace pl2d::framebuffer
