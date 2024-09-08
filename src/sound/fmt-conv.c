
#pragma GCC optimize("O3")

#include "fmtconv-leaf.h"

#include <sound.h>

// 是否为有符号
extern bool _sound_fmt_issigned[];
// 是否为浮点
extern bool _sound_fmt_isfloat[];
// 是否为大端序
extern bool _sound_fmt_isbe[];
// 大小
extern int  _sound_fmt_bytes[];

// 音频采样格式转换

//* ---------------------------------------------------------------------------------------------------
//* 宏 函数定义

#define __fmtconv_(dstname, srcname, dsttype, srctype)                                             \
  void sound_fmt_##srcname##to##dstname(dsttype *_rest dst, const srctype *_rest src,              \
                                        const size_t len)

#define FLT_TRUNC(X) ((X) > 1 ? 1 : ((X) < -1 ? -1 : (X)))

//^ --------------------------------------------------
//^ 浮点转浮点
#define __FntoFn(Fns, Fnd, ...)                                                                    \
  __fmtconv_(F##Fnd, F##Fns, f##Fnd, f##Fns) {                                                     \
    for (size_t i = 0; i < len; i++)                                                               \
      dst[i] = src[i];                                                                             \
  }
//^ 有符号转有符号
#define __SntoSn(Sns, Snd, ...)                                                                    \
  __fmtconv_(S##Snd, S##Sns, i##Snd, i##Sns) {                                                     \
    for (size_t i = 0; i < len; i++)                                                               \
      dst[i] = src[i] / (f32)INT##Sns##_MAX * (f32)INT##Snd##_MAX;                                 \
  }
//^ 无符号转无符号
#define __UntoUn(Uns, Und, ...)                                                                    \
  __fmtconv_(U##Und, U##Uns, u##Und, u##Uns) {                                                     \
    for (size_t i = 0; i < len; i++)                                                               \
      dst[i] = (u##Und)((i##Uns)(src[i] + INT##Uns##_MIN) / (f32)INT##Uns##_MAX *                  \
                        (f32)INT##Und##_MAX) -                                                     \
               INT##Und##_MIN;                                                                     \
  }

//^ --------------------------------------------------
//^ 有符号转浮点
#define __SntoFn(Sn, Fn, ...)                                                                      \
  __fmtconv_(F##Fn, S##Sn, f##Fn, i##Sn) {                                                         \
    for (size_t i = 0; i < len; i++)                                                               \
      dst[i] = src[i] / (f##Fn)INT##Sn##_MAX;                                                      \
  }
//^ 无符号转浮点
#define __UntoFn(Un, Fn, ...)                                                                      \
  __fmtconv_(F##Fn, U##Un, f##Fn, u##Un) {                                                         \
    for (size_t i = 0; i < len; i++)                                                               \
      dst[i] = ((i##Un)src[i] + INT##Un##_MIN) / (f##Fn)INT##Un##_MAX;                             \
  }
//^ 浮点转有符号
#define __FntoSn(Fn, Sn, ...)                                                                      \
  __fmtconv_(S##Sn, F##Fn, i##Sn, f##Fn) {                                                         \
    for (size_t i = 0; i < len; i++)                                                               \
      dst[i] = FLT_TRUNC(src[i]) * (f##Fn)INT##Sn##_MAX;                                           \
  }
//^ 浮点转无符号
#define __FntoUn(Fn, Un, ...)                                                                      \
  __fmtconv_(U##Un, F##Fn, u##Un, f##Fn) {                                                         \
    for (size_t i = 0; i < len; i++)                                                               \
      dst[i] = (u##Un)(FLT_TRUNC(src[i]) * (f##Fn)INT##Un##_MAX) - INT##Un##_MIN;                  \
  }

//^ --------------------------------------------------
//^ 有符号转无符号
#define __SntoUn(Sn, Un, ...)                                                                      \
  __fmtconv_(U##Un, S##Sn, u##Un, i##Sn) {                                                         \
    for (size_t i = 0; i < len; i++)                                                               \
      dst[i] = (u##Un)(src[i] / (f32)INT##Sn##_MAX * (f32)INT##Un##_MAX) - INT##Un##_MIN;          \
  }
//^ 无符号转有符号
#define __UntoSn(Un, Sn, ...)                                                                      \
  __fmtconv_(S##Sn, U##Un, i##Sn, u##Un) {                                                         \
    for (size_t i = 0; i < len; i++)                                                               \
      dst[i] = (src[i] + INT##Un##_MIN) / (f32)INT##Un##_MAX * (f32)INT##Sn##_MAX;                 \
  }

//^ --------------------------------------------------
//^ 有符号转对应无符号
#define __StoUn(Sn, ...)                                                                           \
  __fmtconv_(U##Sn, S##Sn, u##Sn, i##Sn) {                                                         \
    for (size_t i = 0; i < len; i++)                                                               \
      dst[i] = src[i] - INT##Sn##_MIN;                                                             \
  }
//^ 无符号转对应有符号
#define __UtoSn(Sn, ...)                                                                           \
  __fmtconv_(S##Sn, U##Sn, i##Sn, u##Sn) {                                                         \
    for (size_t i = 0; i < len; i++)                                                               \
      dst[i] = src[i] + INT##Sn##_MIN;                                                             \
  }

//^ --------------------------------------------------
//^ 我转换成我自己
#define __to_self(name, type, ...)                                                                 \
  __fmtconv_(name, name, type, type) {                                                             \
    __builtin_memcpy(dst, src, len * sizeof(type));                                                \
  }

#include "fmtconv/1.h"

#include "fmtconv/undef.h"
//* ----------------------------------------------------------------------------------------------------

#define __fmtconv_case_(dstname, srcname)                                                          \
  case SOUND_FMT_##srcname: sound_fmt_##srcname##to##dstname(dst, src, len); return 0

#if defined(__x86_64__) && !NO_EXTFLOAT

#  define __fmtconv_(dstname, dsttype)                                                             \
    int sound_fmt_convto##dstname(dsttype *_rest dst, const void *_rest src,                       \
                                  const sound_pcmfmt_t srcfmt, const size_t len) {                 \
      switch (srcfmt) {                                                                            \
        __fmtconv_case_(dstname, S8);                                                              \
        __fmtconv_case_(dstname, U8);                                                              \
        __fmtconv_case_(dstname, S16);                                                             \
        __fmtconv_case_(dstname, U16);                                                             \
        __fmtconv_case_(dstname, S32);                                                             \
        __fmtconv_case_(dstname, U32);                                                             \
        __fmtconv_case_(dstname, S64);                                                             \
        __fmtconv_case_(dstname, U64);                                                             \
        __fmtconv_case_(dstname, F16);                                                             \
        __fmtconv_case_(dstname, F32);                                                             \
        __fmtconv_case_(dstname, F64);                                                             \
      default: return -1;                                                                          \
      }                                                                                            \
    }

#else

#  define __fmtconv_(dstname, dsttype)                                                             \
    int sound_fmt_convto##dstname(dsttype *_rest dst, const void *_rest src,                       \
                                  const sound_pcmfmt_t srcfmt, const size_t len) {                 \
      switch (srcfmt) {                                                                            \
        __fmtconv_case_(dstname, S8);                                                              \
        __fmtconv_case_(dstname, U8);                                                              \
        __fmtconv_case_(dstname, S16);                                                             \
        __fmtconv_case_(dstname, U16);                                                             \
        __fmtconv_case_(dstname, S32);                                                             \
        __fmtconv_case_(dstname, U32);                                                             \
        __fmtconv_case_(dstname, S64);                                                             \
        __fmtconv_case_(dstname, U64);                                                             \
        __fmtconv_case_(dstname, F32);                                                             \
        __fmtconv_case_(dstname, F64);                                                             \
      default: return -1;                                                                          \
      }                                                                                            \
    }

#endif

#define __fmtconvto_case_(dstname)                                                                 \
  case SOUND_FMT_##dstname: return sound_fmt_convto##dstname(dst, src, srcfmt, len)

#include "fmtconv/2.h"

int sound_fmt_conv(void *_rest dst, const sound_pcmfmt_t dstfmt, const void *_rest src,
                   const sound_pcmfmt_t srcfmt, const size_t len) {
  if (!src || !dst) return -1;
  if (srcfmt < 0 || srcfmt >= SOUND_FMT_CNT) return -1;
  if (dstfmt < 0 || dstfmt >= SOUND_FMT_CNT) return -1;

  if (srcfmt == dstfmt) {
    __builtin_memcpy(dst, src, len * _sound_fmt_bytes[srcfmt]);
    return 0;
  }

  switch (dstfmt) {
    __fmtconvto_case_(S8);
    __fmtconvto_case_(U8);
    __fmtconvto_case_(S16);
    __fmtconvto_case_(U16);
    __fmtconvto_case_(S32);
    __fmtconvto_case_(U32);
    __fmtconvto_case_(S64);
    __fmtconvto_case_(U64);
#if defined(__x86_64__) && !NO_EXTFLOAT
    __fmtconvto_case_(F16);
#endif
    __fmtconvto_case_(F32);
    __fmtconvto_case_(F64);
  default: return -1;
  }
}

#include "fmtconv/undef.h"
//* ----------------------------------------------------------------------------------------------------

#define __fmtconv_itop_(N)                                                                         \
  void sound_fmt_itop##N(uint##N##_t *_rest *_rest dst, const int channels,                        \
                         const uint##N##_t *_rest src, const size_t len) {                         \
    for (int c = 0; c < channels; c++)                                                             \
      for (size_t i = 0; i < len; i++)                                                             \
        dst[c][i] = src[i * channels + c];                                                         \
  }

#define __fmtconv_ptoi_(N)                                                                         \
  void sound_fmt_ptoi##N(uint##N##_t *_rest dst, const uint##N##_t *_rest *_rest src,              \
                         const int channels, const size_t len) {                                   \
    for (int c = 0; c < channels; c++)                                                             \
      for (size_t i = 0; i < len; i++)                                                             \
        dst[i * channels + c] = src[c][i];                                                         \
  }

#include "fmtconv/3.h"

int sound_fmt_itop(void *_rest *_rest dst, const int channels, const void *_rest src,
                   const int nbytes, const size_t len) {
  if (!dst || !src) return -1;
  if (channels <= 0) return -1;
  switch (nbytes) {
  case 1: sound_fmt_itop8((void *)dst, channels, src, len); return 0;
  case 2: sound_fmt_itop16((void *)dst, channels, src, len); return 0;
  case 4: sound_fmt_itop32((void *)dst, channels, src, len); return 0;
  case 8: sound_fmt_itop64((void *)dst, channels, src, len); return 0;
  default: return -1;
  }
}
int sound_fmt_ptoi(void *_rest dst, const void *_rest *_rest src, const int channels,
                   const int nbytes, const size_t len) {
  if (!dst || !src) return -1;
  if (channels <= 0) return -1;
  switch (nbytes) {
  case 1: sound_fmt_ptoi8(dst, (void *)src, channels, len); return 0;
  case 2: sound_fmt_ptoi16(dst, (void *)src, channels, len); return 0;
  case 4: sound_fmt_ptoi32(dst, (void *)src, channels, len); return 0;
  case 8: sound_fmt_ptoi64(dst, (void *)src, channels, len); return 0;
  default: return -1;
  }
}

#include "fmtconv/undef.h"
//* ----------------------------------------------------------------------------------------------------
//* 结束
