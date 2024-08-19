#pragma once
#include <data-structure.h>
#include <misc.h>

typedef struct quantized {
  i16    max;
  i16    min;
  i16    mid;
  i16    nbit;
  size_t len;
  f32   *dataf;
  i16   *datai;
} *quantized_t;

typedef void (*cb_plac_compress_t)(const void *data, size_t size, void *userdata);
typedef void (*cb_plac_decompress_t)(f32 *block, size_t len, void *userdata);

typedef struct plac_compress {
  size_t           block_len;
  f32              vol;
  mdctf_t          mdct;
  struct quantized q;
  mostream_t       stream;
} *plac_compress_t;

typedef struct plac_decompress {
  size_t               block_len;
  mdctf_t              mdct;
  struct quantized     q;
  mistream_t           stream;
  cb_plac_decompress_t callback;
  void                *userdata;
} *plac_decompress_t;

// block_len 应为 1024
plac_compress_t plac_compress_alloc(size_t block_len);
void            plac_compress_free(plac_compress_t plac);
void            plac_write_header(plac_compress_t plac, u16 samplerate, u32 nsamples);
void            plac_write_data(plac_compress_t plac, quantized_t q);
void            plac_compress_block(plac_compress_t plac, f32 *block, size_t len);
void            plac_compress_final(plac_compress_t plac);

// block_len 应为 1024
plac_decompress_t plac_decompress_alloc(const void *buffer, size_t size, size_t block_len);
void              plac_decompress_free(plac_decompress_t plac);
bool              plac_read_header(plac_decompress_t plac, u16 *samplerate, u32 *nsamples);
void              plac_read_data(plac_decompress_t plac, quantized_t q);
bool              plac_decompress_block(plac_decompress_t plac);