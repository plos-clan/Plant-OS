#pragma once
#include "texture.hpp"
#include <libc-base.hpp>

namespace pl2d {

enum class PixFmt : u32 {
  BlackWhite, // 黑白双色
  Grayscale8, // 8位灰度
  Palette16,  // 16色调色板
  Palette256, // 256色调色板

  RGB565, //
  BGR565, //

  // 24位真彩色
  RGB888,
  RGB = RGB888,
  BGR888,
  BGR = BGR888,
  // 32位真彩色
  RGBA8888,
  RGBA = RGBA8888,
  BGRA8888,
  BGRA = BGRA8888,
  ARGB8888,
  ARGB = ARGB8888,
  ABGR8888,
  ABGR = ABGR8888,

  // 浮点
  RGB_FLT,
  BGR_FLT,
  RGBA_FLT,
  BGRA_FLT,
  ARGB_FLT,
  ABGR_FLT,

  // 通道分离
  RGB_Plane,
  RGBA_Plane,
  RGB_FLT_Plane,
  RGBA_FLT_Plane,
};

enum class PalFmt : u32 {
  None,

  // 24位真彩色
  RGB888,
  RGB = RGB888,
  BGR888,
  BGR = BGR888,
  // 32位真彩色
  RGBA8888,
  RGBA = RGBA8888,
  BGRA8888,
  BGRA = BGRA8888,
  ARGB8888,
  ARGB = ARGB8888,
  ABGR8888,
  ABGR = ABGR8888,
};

#if COLOR_USE_BGR
constexpr PixFmt texture_pixfmt = PixFmt::BGRA;
#else
constexpr PixFmt texture_pixfmt = PixFmt::RGBA;
#endif

struct FrameBuffer {
  union {                             //
    void *pix[4] = {};                //
    u8   *pix8[4];                    //
    u16  *pix16[4];                   //
    u32  *pix32[4];                   //
    struct {                          //
      byte *plane1;                   //
      byte *plane2;                   //
      byte *plane3;                   //
      byte *plane4;                   //
    };                                //
  };                                  // 缓冲区
  union {                             //
    u32 plane_size[4] = {};           //
    struct {                          //
      u32 plane1_size;                //
      u32 plane2_size;                //
      u32 plane3_size;                //
      u32 plane4_size;                //
    };                                //
  };                                  // 缓冲区大小
  u32      *pal     = null;           // 调色板
  u32       width   = 0;              // 宽度（可自动计算） width = pitch / size_of_pixel
  u32       height  = 0;              // 高度
  u32       pitch   = 0;              // pitch = width * size_of_pixel
  u32       size    = 0;              // pitch * height
  u16       bpp     = 0;              // 像素深度（可自动计算） size_of_pixel * 8
  u8        padding = 0;              // 按多少字节对齐
  u8        channel = 0;              //
  PixFmt    pixfmt  = texture_pixfmt; // 像素格式
  PalFmt    palfmt  = PalFmt::None;   // 像素格式
  bool      alpha   = false;          // 是否有alpha通道（可自动计算）
  bool      plane   = false;          // 是否通道分离
  bool      ready   = false;          //
  TextureB *tex_b   = null;           // 不使用
  TextureB *tex_f   = null;           // 不使用

  FrameBuffer() = default;
  FrameBuffer(void *data, u32 width, u32 height, PixFmt pixfmt)
      : pix(data, null, null, null), width(width), height(height), pixfmt(pixfmt) {
    init();
  }
  FrameBuffer(const FrameBuffer &)     = delete;
  FrameBuffer(FrameBuffer &&) noexcept = default;

  auto operator=(const FrameBuffer &) -> FrameBuffer     & = delete;
  auto operator=(FrameBuffer &&) noexcept -> FrameBuffer & = default;

  void reset() {
    *this = {};
  }

  // 设置好需要的参数后使用 init 来自动填充其它参数
  auto init() -> int; // 返回 0: 正常，<0:异常

  void clear();       // 全部数据清零
  void clear(byte b); // 全部数据设置为指定值

  // rect 是要更新的区域
  auto flush(const TextureB &tex) -> int;
  auto flush(const TextureF &tex) -> int;
  auto flush(const TextureB &tex, const Rect &rect) -> int;
  auto flush(const TextureF &tex, const Rect &rect) -> int;

  auto copy_to(TextureB &tex) const -> int;
  auto copy_to(TextureF &tex) const -> int;
  auto copy_to(TextureB &tex, const Rect &rect) const -> int;
  auto copy_to(TextureF &tex, const Rect &rect) const -> int;

  void init_texture(TextureB &tex);
  void init_texture(TextureF &tex);
  auto new_texture_b() -> TextureB *;
  auto new_texture_f() -> TextureF *;
};

} // namespace pl2d
