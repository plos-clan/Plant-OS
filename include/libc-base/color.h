#pragma once
#include "00-include.h"

// 抽象的 RGB 宏

#if LITTLE_ENDIAN
#  if COLOR_READABLE_HEX
#    define RGBA(r, g, b, a) ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | (u8)(a))
#    define RGB(r, g, b)     ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | 255)
#    define BGRA(b, g, r, a) ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | (u8)(a))
#    define BGR(b, g, r)     ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | 255)
#    define DESTRUCT_RGB(_c_, _r_, _g_, _b_)                                                       \
      byte _r_ = _c_ >> 24;                                                                        \
      byte _g_ = _c_ >> 16;                                                                        \
      byte _b_ = _c_ >> 8;
#    define DESTRUCT_RGBA(_c_, _r_, _g_, _b_, _a_)                                                 \
      byte _r_ = _c_ >> 24;                                                                        \
      byte _g_ = _c_ >> 16;                                                                        \
      byte _b_ = _c_ >> 8;                                                                         \
      byte _a_ = _c_;
#  elif COLOR_USE_BGR
#    define RGBA(r, g, b, a) ((u32)(u8)(a) << 24 | (u32)(u8)(r) << 16 | (u32)(u8)(g) << 8 | (u8)(b))
#    define RGB(r, g, b)     ((u32)255 << 24 | (u32)(u8)(r) << 16 | (u32)(u8)(g) << 8 | (u8)(b))
#    define BGRA(b, g, r, a) ((u32)(u8)(a) << 24 | (u32)(u8)(r) << 16 | (u32)(u8)(g) << 8 | (u8)(b))
#    define BGR(b, g, r)     ((u32)255 << 24 | (u32)(u8)(r) << 16 | (u32)(u8)(g) << 8 | (u8)(b))
#    define DESTRUCT_RGB(_c_, _r_, _g_, _b_)                                                       \
      byte _b_ = _c_;                                                                              \
      byte _g_ = _c_ >> 8;                                                                         \
      byte _r_ = _c_ >> 16;
#    define DESTRUCT_RGBA(_c_, _r_, _g_, _b_, _a_)                                                 \
      byte _b_ = _c_;                                                                              \
      byte _g_ = _c_ >> 8;                                                                         \
      byte _r_ = _c_ >> 16;                                                                        \
      byte _a_ = _c_ >> 24;
#  else
#    define RGBA(r, g, b, a) ((u32)(u8)(a) << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define RGB(r, g, b)     ((u32)255 << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define BGRA(b, g, r, a) ((u32)(u8)(a) << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define BGR(b, g, r)     ((u32)255 << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define DESTRUCT_RGB(_c_, _r_, _g_, _b_)                                                       \
      byte _r_ = _c_;                                                                              \
      byte _g_ = _c_ >> 8;                                                                         \
      byte _b_ = _c_ >> 16;
#    define DESTRUCT_RGBA(_c_, _r_, _g_, _b_, _a_)                                                 \
      byte _r_ = _c_;                                                                              \
      byte _g_ = _c_ >> 8;                                                                         \
      byte _b_ = _c_ >> 16;                                                                        \
      byte _a_ = _c_ >> 24;
#  endif
#endif

#if BIG_ENDIAN
#  if COLOR_READABLE_HEX
#    define RGBA(r, g, b, a) ((u32)(u8)(a) << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define RGB(r, g, b)     ((u32)255 << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define BGRA(b, g, r, a) ((u32)(u8)(a) << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define BGR(b, g, r)     ((u32)255 << 24 | (u32)(u8)(b) << 16 | (u32)(u8)(g) << 8 | (u8)(r))
#    define DESTRUCT_RGB(_c_, _r_, _g_, _b_)                                                       \
      byte _r_ = _c_;                                                                              \
      byte _g_ = _c_ >> 8;                                                                         \
      byte _b_ = _c_ >> 16;
#    define DESTRUCT_RGBA(_c_, _r_, _g_, _b_, _a_)                                                 \
      byte _r_ = _c_;                                                                              \
      byte _g_ = _c_ >> 8;                                                                         \
      byte _b_ = _c_ >> 16;                                                                        \
      byte _a_ = _c_ >> 24;
#  elif COLOR_USE_BGR
#    define RGBA(r, g, b, a) ((u32)(u8)(b) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(r) << 8 | (u8)(a))
#    define RGB(r, g, b)     ((u32)(u8)(b) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(r) << 8 | 255)
#    define BGRA(b, g, r, a) ((u32)(u8)(b) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(r) << 8 | (u8)(a))
#    define BGR(b, g, r)     ((u32)(u8)(b) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(r) << 8 | 255)
#    define DESTRUCT_RGB(_c_, _r_, _g_, _b_)                                                       \
      byte _b_ = _c_ >> 24;                                                                        \
      byte _g_ = _c_ >> 16;                                                                        \
      byte _r_ = _c_ >> 8;
#    define DESTRUCT_RGBA(_c_, _r_, _g_, _b_, _a_)                                                 \
      byte _b_ = _c_ >> 24;                                                                        \
      byte _g_ = _c_ >> 16;                                                                        \
      byte _r_ = _c_ >> 8;                                                                         \
      byte _a_ = _c_;
#  else
#    define RGBA(r, g, b, a) ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | (u8)(a))
#    define RGB(r, g, b)     ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | 255)
#    define BGRA(b, g, r, a) ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | (u8)(a))
#    define BGR(b, g, r)     ((u32)(u8)(r) << 24 | (u32)(u8)(g) << 16 | (u32)(u8)(b) << 8 | 255)
#    define DESTRUCT_RGB(_c_, _r_, _g_, _b_)                                                       \
      byte _r_ = _c_ >> 24;                                                                        \
      byte _g_ = _c_ >> 16;                                                                        \
      byte _b_ = _c_ >> 8;
#    define DESTRUCT_RGBA(_c_, _r_, _g_, _b_, _a_)                                                 \
      byte _r_ = _c_ >> 24;                                                                        \
      byte _g_ = _c_ >> 16;                                                                        \
      byte _b_ = _c_ >> 8;                                                                         \
      byte _a_ = _c_;
#  endif
#endif
