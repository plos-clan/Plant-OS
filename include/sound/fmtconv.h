#pragma once
#include "fmt.h"
#include <define.h>

// 音频采样格式转换

int sound_fmt_conv(void *_rest dst, sound_pcmfmt_t dstfmt, const void *_rest src,
                   sound_pcmfmt_t srcfmt, size_t len) __nnull(1, 3);

int sound_fmt_itop(void *_rest *_rest dst, int channels, const void *_rest src, int nbytes,
                   size_t len) __THROW __nnull(1, 3);
int sound_fmt_ptoi(void *_rest dst, const void *_rest *_rest src, int channels, int nbytes,
                   size_t len) __THROW __nnull(1, 2);

typedef struct ImaAdpcmCtx {
  int index;
  int prev_sample;
} ImaAdpcmCtx;

void sound_ima_adpcm_encode(ImaAdpcmCtx *ctx, void *dst, const i16 *src, size_t len)
    __nnull(1, 2, 3);
void sound_ima_adpcm_decode(ImaAdpcmCtx *ctx, i16 *dst, const void *src, size_t len)
    __nnull(1, 2, 3);
