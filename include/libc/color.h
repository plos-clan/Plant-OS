#pragma once
#include <define.h>
#include <type.h>

#if COLOR_READABLE_HEX
#  if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define RGBA(r, g, b, a) ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | (u8)(a))
#    define RGB(r, g, b)     ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | 255)
#    define BGRA(b, g, r, a) ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | (u8)(a))
#    define BGR(b, g, r)     ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | 255)
#  else
#    define RGBA(r, g, b, a) ((u32)(u8)(a) << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define RGB(r, g, b)     ((u32)255 << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define BGRA(b, g, r, a) ((u32)(u8)(a) << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define BGR(b, g, r)     ((u32)255 << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#  endif
#elif COLOR_USE_BGR
#  if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define RGBA(r, g, b, a) ((u32)(u8)(a) << 24 | (u32)(u8)(r) << 16 | (u32)(u8)(g) << 8 | (u8)(b))
#    define RGB(r, g, b)     ((u32)255 << 24 | (u32)(u8)(r) << 16 | (u32)(u8)(g) << 8 | (u8)(b))
#    define BGRA(b, g, r, a) ((u32)(u8)(a) << 24 | (u32)(u8)(r) << 16 | (u32)(u8)(g) << 8 | (u8)(b))
#    define BGR(b, g, r)     ((u32)255 << 24 | (u32)(u8)(r) << 16 | (u32)(u8)(g) << 8 | (u8)(b))
#  else
#    define RGBA(r, g, b, a) ((u32)(u8)(b) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(r) << 8 | (u8)(a))
#    define RGB(r, g, b)     ((u32)(u8)(b) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(r) << 8 | 255)
#    define BGRA(b, g, r, a) ((u32)(u8)(b) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(r) << 8 | (u8)(a))
#    define BGR(b, g, r)     ((u32)(u8)(b) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(r) << 8 | 255)
#  endif
#else
#  if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define RGBA(r, g, b, a) ((u32)(u8)(a) << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define RGB(r, g, b)     ((u32)255 << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define BGRA(b, g, r, a) ((u32)(u8)(a) << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define BGR(b, g, r)     ((u32)255 << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#  else
#    define RGBA(r, g, b, a) ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | (u8)(a))
#    define RGB(r, g, b)     ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | 255)
#    define BGRA(b, g, r, a) ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | (u8)(a))
#    define BGR(b, g, r)     ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | 255)
#  endif
#endif

dlimport u32 color_from_name(cstr name);
