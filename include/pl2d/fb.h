#pragma once
#include <libc-base.h>

typedef enum pl2d_PixFmt {
  pl2d_PixFmt_BlackWhite,
  pl2d_PixFmt_Grayscale8,
  pl2d_PixFmt_Palette16,
  pl2d_PixFmt_Palette256,
  pl2d_PixFmt_RGB565,
  pl2d_PixFmt_BGR565,
  pl2d_PixFmt_RGB888,
  pl2d_PixFmt_RGB = pl2d_PixFmt_RGB888,
  pl2d_PixFmt_BGR888,
  pl2d_PixFmt_BGR = pl2d_PixFmt_BGR888,
  pl2d_PixFmt_RGBA8888,
  pl2d_PixFmt_RGBA = pl2d_PixFmt_RGBA8888,
  pl2d_PixFmt_BGRA8888,
  pl2d_PixFmt_BGRA = pl2d_PixFmt_BGRA8888,
  pl2d_PixFmt_ARGB8888,
  pl2d_PixFmt_ARGB = pl2d_PixFmt_ARGB8888,
  pl2d_PixFmt_ABGR8888,
  pl2d_PixFmt_ABGR = pl2d_PixFmt_ABGR8888,
  pl2d_PixFmt_RGB_FLT,
  pl2d_PixFmt_BGR_FLT,
  pl2d_PixFmt_RGBA_FLT,
  pl2d_PixFmt_BGRA_FLT,
  pl2d_PixFmt_ARGB_FLT,
  pl2d_PixFmt_ABGR_FLT,
  pl2d_PixFmt_RGB_Plane,
  pl2d_PixFmt_RGBA_Plane,
  pl2d_PixFmt_RGB_FLT_Plane,
  pl2d_PixFmt_RGBA_FLT_Plane,
} pl2d_PixFmt;

typedef enum pl2d_PalFmt {
  pl2d_PalFmt_None,
  pl2d_PalFmt_RGB888,
  pl2d_PalFmt_RGB = pl2d_PalFmt_RGB888,
  pl2d_PalFmt_BGR888,
  pl2d_PalFmt_BGR = pl2d_PalFmt_BGR888,
  pl2d_PalFmt_RGBA8888,
  pl2d_PalFmt_RGBA = pl2d_PalFmt_RGBA8888,
  pl2d_PalFmt_BGRA8888,
  pl2d_PalFmt_BGRA = pl2d_PalFmt_BGRA8888,
  pl2d_PalFmt_ARGB8888,
  pl2d_PalFmt_ARGB = pl2d_PalFmt_ARGB8888,
  pl2d_PalFmt_ABGR8888,
  pl2d_PalFmt_ABGR = pl2d_PalFmt_ABGR8888,
} pl2d_PalFmt;

#ifndef __cplusplus
#  if COLOR_USE_BGR
#    define texture_pixfmt pl2d_PixFmt_BGRA
#  else
#    define texture_pixfmt pl2d_PixFmt_RGBA
#  endif
#endif
