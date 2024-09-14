#pragma once
#include "fmt.h"
#include <define.h>

// 音频采样格式转换

extern int sound_fmt_conv(void *_rest dst, sound_pcmfmt_t dstfmt, const void *_rest src,
                          sound_pcmfmt_t srcfmt, size_t len);

extern int sound_fmt_itop(void *_rest *_rest dst, int channels, const void *_rest src, int nbytes,
                          size_t len) __THROW;
extern int sound_fmt_ptoi(void *_rest dst, const void *_rest *_rest src, int channels, int nbytes,
                          size_t len) __THROW;

typedef struct ImaAdpcmCtx ImaAdpcmCtx;
struct ImaAdpcmCtx {
  int index;
  int prev_sample;
};

void sound_ima_adpcm_encode(ImaAdpcmCtx *ctx, void *dst, i16 *src, size_t len);
void sound_ima_adpcm_decode(ImaAdpcmCtx *ctx, i16 *dst, void *src, size_t len);
