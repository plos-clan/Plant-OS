#pragma once
#include <pl2d.hpp>

namespace pl2d {

auto FrameBuffer::init() -> int {
  if (ready) return 0;

  if (pixfmt == PixFmt::BlackWhite) ERROR(1, "暂不支持");
  if (pixfmt == PixFmt::Palette16) ERROR(1, "暂不支持");
  if (pixfmt == PixFmt::Palette256) ERROR(1, "暂不支持");

  // 检查 bpp 的设置
  auto old_bpp = bpp;
  switch (pixfmt) {
  case PixFmt::BlackWhite: bpp = 1; break;
  case PixFmt::Grayscale8: bpp = 8; break;
  case PixFmt::Palette16: bpp = 4; break;
  case PixFmt::Palette256: bpp = 8; break;
  case PixFmt::RGB565:
  case PixFmt::BGR565: bpp = 16; break;
  case PixFmt::RGB:
  case PixFmt::BGR: bpp = 24; break;
  case PixFmt::RGBA:
  case PixFmt::BGRA:
  case PixFmt::ARGB:
  case PixFmt::ABGR: bpp = 32; break;
  case PixFmt::RGB_FLT:
  case PixFmt::BGR_FLT: bpp = 96; break;
  case PixFmt::RGBA_FLT:
  case PixFmt::BGRA_FLT:
  case PixFmt::ARGB_FLT:
  case PixFmt::ABGR_FLT: bpp = 128; break;
  case PixFmt::RGB_Plane: bpp = 24; break;
  case PixFmt::RGBA_Plane: bpp = 32; break;
  case PixFmt::RGB_FLT_Plane: bpp = 96; break;
  case PixFmt::RGBA_FLT_Plane: bpp = 128; break;
  default: ERROR(1, "未知的像素格式");
  }
  if (old_bpp > 0 && bpp != old_bpp) ERROR(1, "bpp 与 fmt 不匹配");

  // 检查透明通道的设置
  if (alpha) {
    switch (pixfmt) {
    case PixFmt::BlackWhite:
    case PixFmt::Grayscale8:
    case PixFmt::RGB565:
    case PixFmt::BGR565:
    case PixFmt::RGB:
    case PixFmt::BGR:
    case PixFmt::RGB_FLT:
    case PixFmt::BGR_FLT:
    case PixFmt::RGB_Plane:
    case PixFmt::RGB_FLT_Plane: ERROR(1, "像素格式 与 alpha 的设置不相符");
    default: break;
    }
  } else {
    switch (pixfmt) {
    case PixFmt::RGBA:
    case PixFmt::BGRA:
    case PixFmt::ARGB:
    case PixFmt::ABGR:
    case PixFmt::RGBA_FLT:
    case PixFmt::BGRA_FLT:
    case PixFmt::ARGB_FLT:
    case PixFmt::ABGR_FLT:
    case PixFmt::RGBA_Plane:
    case PixFmt::RGBA_FLT_Plane: alpha = true; break;
    default: break;
    }
  }

  // 检查并设置对齐
  if (padding == 0) padding = bpp / 8;
  if (bpp >= 8 && padding * 8 < bpp) ERROR(1, "padding 必须大于像素占用的空间大小");
  if (pixfmt == PixFmt::RGB565 || pixfmt == PixFmt::BGR565) {
    if (padding != 2) ERROR(1, "RGB565 与 BGR565 暂不支持 padding");
  }

  // 计算pitch
  if (pitch == 0) pitch = (width * bpp + 7) / 8;
  if (pitch == 0) ERROR(1, "pitch 与 width 不能同时为 0");

  // 计算width
  if (width == 0) width = pitch * 8 / bpp;
  if (width == 0) ERROR(1, "pitch 与 width 不能同时为 0");

  if (width * bpp > pitch * 8) ERROR(1, "一行像素占用的空间不能大于 pitch");

  // 计算size
  u32 _size = pitch * height;
  if (size == 0) size = _size;
  if (size != _size) ERROR(1, "size 变量的大小与计算值不同");

  if (pixfmt == PixFmt::RGB_Plane) ERROR(1, "当前暂不支持通道分离");
  if (pixfmt == PixFmt::RGBA_Plane) ERROR(1, "当前暂不支持通道分离");
  if (pixfmt == PixFmt::RGB_FLT_Plane) ERROR(1, "当前暂不支持通道分离");
  if (pixfmt == PixFmt::RGBA_FLT_Plane) ERROR(1, "当前暂不支持通道分离");
  if (plane) ERROR(1, "当前暂不支持通道分离");

  if (palfmt != PalFmt::None) ERROR(1, "当前暂不支持调色板");

  ready = true;
  return 0;
}

void FrameBuffer::clear() {
  for (auto &k : pix8) {
    if (k == null) continue;
    for (u32 i = 0; i < size; i++) {
      k[i] = 0;
    }
  }
}

void FrameBuffer::clear(byte b) {
  for (auto &k : pix8) {
    if (k == null) continue;
    for (u32 i = 0; i < size; i++) {
      k[i] = b;
    }
  }
}

void FrameBuffer::init_texture(pl2d::TextureB &tex) {
  if (pixfmt == texture_pixfmt && padding == 4 && pitch % 4 == 0) {
    tex = pl2d::TextureB((pl2d::PixelB *)pix[0], width, height, pitch / 4);
  } else {
    tex = pl2d::TextureB(width, height);
  }
}
void FrameBuffer::init_texture(pl2d::TextureF &tex) {
  tex = pl2d::TextureF(width, height);
}
auto FrameBuffer::new_texture_b() -> pl2d::TextureB * {
  if (pixfmt == texture_pixfmt && padding == 4 && pitch % 4 == 0) {
    return new pl2d::TextureB((pl2d::PixelB *)pix[0], width, height, pitch / 4);
  } else {
    return new pl2d::TextureB(width, height);
  }
}
auto FrameBuffer::new_texture_f() -> pl2d::TextureF * {
  return new pl2d::TextureF(width, height);
}

} // namespace pl2d
