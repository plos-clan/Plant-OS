#pragma once
#include <define.h>

// %% no-extern-c %%

#ifndef __cplusplus
typedef enum {
#  define FMT(f) SOUND_FMT_##f
#else
enum class SoundFmt : i32 {
#  define FMT(f) f
#endif
  FMT(ukn) = -1,
  //- 8bit / 1byte
  FMT(S8),
  FMT(U8),
  //- 16bit / 2byte
  FMT(S16L),
  FMT(S16B),
  FMT(U16L),
  FMT(U16B),
  //- 24bit / 3byte
  FMT(S24L),
  FMT(S24B),
  FMT(U24L),
  FMT(U24B),
  //- 24bit / 4byte (low 3byte)
  FMT(S24L32),
  FMT(S24B32),
  FMT(U24L32),
  FMT(U24B32),
  //- 32bit / 4byte
  FMT(S32L),
  FMT(S32B),
  FMT(U32L),
  FMT(U32B),
  //- 64bit / 8byte
  FMT(S64L),
  FMT(S64B),
  FMT(U64L),
  FMT(U64B),
  //- 16bit / 2byte  <- float
  FMT(F16L),
  FMT(F16B),
  //- 32bit / 4byte  <- float
  FMT(F32L),
  FMT(F32B),
  //- 64bit / 8byte  <- float
  FMT(F64L),
  FMT(F64B),
  //- 8bit / 1byte   <- Mu-Law
  FMT(MU_LAW),
  //- 8bit / 1byte   <- A-Law
  FMT(A_LAW),
  //- 4bit / 0.5byte <- Ima-ADPCM
  FMT(IMA_ADPCM),
  //- 计数
  FMT(CNT),
  //- 按通道存储 (如果是就给 fmt 加上此值)
  FMT(PLANE) = 32,

#if LITTLE_ENDIAN
  FMT(S16)    = FMT(S16L),
  FMT(U16)    = FMT(U16L),
  FMT(S24)    = FMT(S24L),
  FMT(U24)    = FMT(U24L),
  FMT(S24_32) = FMT(S24L32),
  FMT(U24_32) = FMT(U24L32),
  FMT(S32)    = FMT(S32L),
  FMT(U32)    = FMT(U32L),
  FMT(S64)    = FMT(S64L),
  FMT(U64)    = FMT(U64L),
  FMT(F16)    = FMT(F16L),
  FMT(F32)    = FMT(F32L),
  FMT(F64)    = FMT(F64L),
#elif BIG_ENDIAN
  FMT(S16)    = FMT(S16B),
  FMT(U16)    = FMT(U16B),
  FMT(S24)    = FMT(S24B),
  FMT(U24)    = FMT(U24B),
  FMT(S24_32) = FMT(S24B32),
  FMT(U24_32) = FMT(U24B32),
  FMT(S32)    = FMT(S32B),
  FMT(U32)    = FMT(U32B),
  FMT(S64)    = FMT(S64B),
  FMT(U64)    = FMT(U64B),
  FMT(F16)    = FMT(F16B),
  FMT(F32)    = FMT(F32B),
  FMT(F64)    = FMT(F64B),
#endif
#ifndef __cplusplus
} sound_pcmfmt_t;
#else
};
#endif
#undef FMT

dlimport bool sound_fmt_issigned(sound_pcmfmt_t fmt);
dlimport bool sound_fmt_isfloat(sound_pcmfmt_t fmt);
dlimport bool sound_fmt_isbe(sound_pcmfmt_t fmt);
dlimport int  sound_fmt_bytes(sound_pcmfmt_t fmt);
