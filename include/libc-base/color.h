#pragma once
#include "00-include.h"

#if LITTLE_ENDIAN
#  if COLOR_READABLE_HEX
#    define RGBA(r, g, b, a) ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | (u8)(a))
#    define RGB(r, g, b)     ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | 255)
#    define BGRA(b, g, r, a) ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | (u8)(a))
#    define BGR(b, g, r)     ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | 255)
#  elif COLOR_USE_BGR
#    define RGBA(r, g, b, a) ((u32)(u8)(a) << 24 | (u32)(u8)(r) << 16 | (u32)(u8)(g) << 8 | (u8)(b))
#    define RGB(r, g, b)     ((u32)255 << 24 | (u32)(u8)(r) << 16 | (u32)(u8)(g) << 8 | (u8)(b))
#    define BGRA(b, g, r, a) ((u32)(u8)(a) << 24 | (u32)(u8)(r) << 16 | (u32)(u8)(g) << 8 | (u8)(b))
#    define BGR(b, g, r)     ((u32)255 << 24 | (u32)(u8)(r) << 16 | (u32)(u8)(g) << 8 | (u8)(b))
#  else
#    define RGBA(r, g, b, a) ((u32)(u8)(a) << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define RGB(r, g, b)     ((u32)255 << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define BGRA(b, g, r, a) ((u32)(u8)(a) << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define BGR(b, g, r)     ((u32)255 << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#  endif
#endif

#if BIG_ENDIAN
#  if COLOR_READABLE_HEX
#    define RGBA(r, g, b, a) ((u32)(u8)(a) << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define RGB(r, g, b)     ((u32)255 << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define BGRA(b, g, r, a) ((u32)(u8)(a) << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define BGR(b, g, r)     ((u32)255 << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#  elif COLOR_USE_BGR
#    define RGBA(r, g, b, a) ((u32)(u8)(b) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(r) << 8 | (u8)(a))
#    define RGB(r, g, b)     ((u32)(u8)(b) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(r) << 8 | 255)
#    define BGRA(b, g, r, a) ((u32)(u8)(b) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(r) << 8 | (u8)(a))
#    define BGR(b, g, r)     ((u32)(u8)(b) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(r) << 8 | 255)
#  else
#    define RGBA(r, g, b, a) ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | (u8)(a))
#    define RGB(r, g, b)     ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | 255)
#    define BGRA(b, g, r, a) ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | (u8)(a))
#    define BGR(b, g, r)     ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | 255)
#  endif
#endif
