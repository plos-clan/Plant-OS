#pragma once

// 音频采样格式转换

#include <sound.h>

//* ---------------------------------------------------------------------------------------------------
//* 宏 函数声明

#define __fmtconv_(dstname, srcname, dsttype, srctype)                                             \
  extern void sound_fmt##srcname##to##dstname(dsttype *_rest dst, const srctype *_rest src,        \
                                              size_t len) __THROW __nnull(1, 2)                    \
      __attr_writeonly(1, 3) __attr_readonly(2, 3)

//^ --------------------------------------------------
//^ 浮点转浮点
#define __FntoFn(Fns, Fnd) __fmtconv_(F##Fnd, F##Fns, f##Fnd, f##Fns)
//^ 有符号转有符号
#define __SntoSn(Sns, Snd) __fmtconv_(S##Snd, S##Sns, i##Snd, i##Sns)
//^ 无符号转无符号
#define __UntoUn(Uns, Und) __fmtconv_(U##Und, U##Uns, u##Und, u##Uns)

//^ --------------------------------------------------
//^ 有符号转浮点
#define __SntoFn(Sn, Fn) __fmtconv_(F##Fn, S##Sn, f##Fn, i##Sn)
//^ 无符号转浮点
#define __UntoFn(Un, Fn) __fmtconv_(F##Fn, U##Un, f##Fn, u##Un)
//^ 浮点转有符号
#define __FntoSn(Fn, Sn) __fmtconv_(S##Sn, F##Fn, i##Sn, f##Fn)
//^ 浮点转无符号
#define __FntoUn(Fn, Un) __fmtconv_(U##Un, F##Fn, u##Un, f##Fn)

//^ --------------------------------------------------
//^ 有符号转无符号
#define __SntoUn(Sn, Un) __fmtconv_(U##Un, S##Sn, u##Un, i##Sn)
//^ 无符号转有符号
#define __UntoSn(Un, Sn) __fmtconv_(S##Sn, U##Un, i##Sn, u##Un)

//^ --------------------------------------------------
//^ 有符号转对应无符号
#define __StoUn(Sn) __SntoUn(Sn, Sn)
//^ 无符号转对应有符号
#define __UtoSn(Sn) __UntoSn(Sn, Sn)

//^ --------------------------------------------------
//^ 我转换成我自己
#define __to_self(name, type) __fmtconv_(name, name, type, type)

#include "fmtconv/1.h"

#include "fmtconv/undef.h"
//* ----------------------------------------------------------------------------------------------------

#define __fmtconv_(dstname, dsttype)                                                               \
  extern int sound_fmt_convto##dstname(dsttype *_rest dst, const void *_rest src,                  \
                                       sound_pcmfmt_t srcfmt, size_t len) __THROW __nnull(1, 2)    \
      __attr_writeonly(1, 4) __attr_readonly(2, 4)

#include "fmtconv/2.h"

#include "fmtconv/undef.h"
//* ----------------------------------------------------------------------------------------------------

#define __fmtconv_itop_(N)                                                                         \
  extern void sound_fmt_itop##N(uint##N##_t *_rest *_rest dst, int channels,                       \
                                const uint##N##_t *_rest src, size_t len) __THROW __nnull(1, 3)    \
      __attr_writeonly(1, 2) __attr_readonly(3)

#define __fmtconv_ptoi_(N)                                                                         \
  extern void sound_fmt_ptoi##N(uint##N##_t *_rest dst, const uint##N##_t *_rest *_rest src,       \
                                int channels, size_t len) __THROW __nnull(1, 2)                    \
      __attr_writeonly(1) __attr_readonly(2, 3)

#include "fmtconv/3.h"

#include "fmtconv/undef.h"
//* ----------------------------------------------------------------------------------------------------
//* 头文件结束
