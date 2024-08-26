#pragma once

// 音频采样格式转换

#include <sound.h>

extern int sound_fmt_conv(void *_rest dst, sound_pcmfmt_t dstfmt, const void *_rest src,
                          sound_pcmfmt_t srcfmt, size_t len);

extern int sound_fmt_itop(void *_rest *_rest dst, int channels, const void *_rest src, int nbytes,
                          size_t len) __THROW;
extern int sound_fmt_ptoi(void *_rest dst, const void *_rest *_rest src, int channels, int nbytes,
                          size_t len) __THROW;
