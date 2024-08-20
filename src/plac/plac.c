// This code is released under the MIT License

// author: copi143

#include <plac.h>

#include "mulaw.h"
#include "quantize.h"
#include "volume.h"

void _plac_compress_block(f32 *block, void *_plac);

void _plac_decompress_block(f32 *block, void *_plac);

#define block_len 2048

plac_compress_t plac_compress_alloc() {
  plac_compress_t plac = malloc(sizeof(struct plac_compress));
  if (plac == null) return null;
  plac->mdct           = mdctf_alloc(2 * block_len, false, _plac_compress_block);
  plac->mdct->userdata = plac;
  plac->q.max          = 0;
  plac->q.min          = 0;
  plac->q.mid          = 0;
  plac->q.nbit         = 0;
  plac->q.len          = block_len;
  plac->q.dataf        = null;
  plac->q.datai        = malloc(block_len * 2);
  plac->stream         = mostream_alloc(1024);
  if (plac->q.datai == null) {
    free(plac);
    return null;
  }
  return plac;
}

void plac_compress_free(plac_compress_t plac) {
  if (plac == null) return;
  mdctf_free(plac->mdct);
  mostream_free(plac->stream);
  free(plac->q.datai);
  free(plac);
}

void plac_write_header_v0(plac_compress_t plac, u16 samplerate, u32 nsamples) {
  u32 magic = MAGIC32('p', 'l', 'a', 'c');
  mostream_write(plac->stream, &magic, 4);
  u16 version = 0;
  mostream_write(plac->stream, &version, 2);
  mostream_write(plac->stream, &samplerate, 2);
  mostream_write(plac->stream, &nsamples, 4);
}

void plac_write_header_v1(plac_compress_t plac, u32 samplerate, u64 nsamples) {
  u32 magic = MAGIC32('p', 'l', 'a', 'c');
  mostream_write(plac->stream, &magic, 4);
  u16 version = 1;
  mostream_write(plac->stream, &version, 2);
  mostream_write(plac->stream, &samplerate, 4);
  mostream_write(plac->stream, &nsamples, 8);
}

void plac_write_header(plac_compress_t plac, u32 samplerate, u64 nsamples) {
  plac_write_header_v1(samplerate, nsamples);
}

void plac_write_data(plac_compress_t plac, quantized_t q) {
  mostream_write(plac->stream, &q->nbit, 2);
  mostream_write(plac->stream, &q->max, 2);
  if (q->nbit == 0) return;
  mobitstream_t s = mobitstream_alloc(1024);
  mobitstream_write_bits(s, q->min, 16);
  mobitstream_write_bits(s, q->mid, 16);
  for (size_t i = 0; i < q->len; i++) {
    const i16 data = q->datai[i] - q->mid;
    mobitstream_write_bit(s, data != 0);
  }
  for (size_t i = 0; i < q->len; i++) {
    const i16 data = q->datai[i] - q->mid;
    if (data != 0) mobitstream_write_bits(s, data, q->nbit);
  }
  size_t size = s->size + (s->bit_pos ? 1 : 0);
  mostream_write(plac->stream, &size, 2);
  mostream_write(plac->stream, s->buf, size);
  mobitstream_free(s);
}

void _plac_compress_block(f32 *block, void *_plac) {
  plac_compress_t plac = _plac;
  volume_fine_tuning(block, block_len);
  mulaw_compress(block, block_len);
  plac->q.dataf = block;
  best_quantize(&plac->q, 0, 15, .001953125);
  plac_write_data(plac, &plac->q);
}

void plac_compress_block(plac_compress_t plac, f32 *block, size_t len) {
  mdctf_put(plac->mdct, block, len);
}

void plac_compress_final(plac_compress_t plac) {
  mdctf_final(plac->mdct);
}

plac_decompress_t plac_decompress_alloc(const void *buffer, size_t size) {
  plac_decompress_t plac = malloc(sizeof(struct plac_decompress));
  if (plac == null) return null;
  plac->mdct           = mdctf_alloc(2 * block_len, true, _plac_decompress_block);
  plac->mdct->userdata = plac;
  plac->q.max          = 0;
  plac->q.min          = 0;
  plac->q.mid          = 0;
  plac->q.nbit         = 0;
  plac->q.len          = block_len;
  plac->q.dataf        = malloc(block_len * 4);
  plac->q.datai        = malloc(block_len * 2);
  plac->stream         = mistream_alloc(buffer, size);
  if (plac->q.datai == null) {
    free(plac);
    return null;
  }
  return plac;
}

void plac_decompress_free(plac_decompress_t plac) {
  if (plac == null) return;
  mdctf_free(plac->mdct);
  mistream_free(plac->stream);
  free(plac->q.dataf);
  free(plac->q.datai);
  free(plac);
}

bool plac_read_header(plac_decompress_t plac, u32 *samplerate, u64 *nsamples) {
  u32 magic;
  mistream_read(plac->stream, &magic, 4);
  if (magic != MAGIC32('p', 'l', 'a', 'c')) return false;
  u16 version;
  mistream_read(plac->stream, &version, 2);
  if (version >= 3) return false;
  *samplerate = 0, *nsamples = 0;
  if (version == 0) {
    mistream_read(plac->stream, samplerate, 2);
    mistream_read(plac->stream, nsamples, 4);
  } else {
    mistream_read(plac->stream, samplerate, 4);
    mistream_read(plac->stream, nsamples, 8);
  }
  return true;
}

void plac_read_data(plac_decompress_t plac, quantized_t q) {
  mistream_read(plac->stream, &q->nbit, 2);
  mistream_read(plac->stream, &q->max, 2);
  if (q->nbit == 0) return;
  size_t size = 0;
  mistream_read(plac->stream, &size, 2);
  void *buf = malloc(size);
  mistream_read(plac->stream, buf, size);
  mibitstream_t s = mibitstream_alloc(buf, size);
  q->min          = mibitstream_read_bits(s, 16);
  q->mid          = mibitstream_read_bits(s, 16);
  bool *bitmap    = malloc(q->len);
  for (size_t i = 0; i < q->len; i++) {
    bitmap[i] = mibitstream_read_bit(s);
  }
  for (size_t i = 0; i < q->len; i++) {
    const i16 data = bitmap[i] ? mibitstream_read_bitsi(s, q->nbit) : 0;
    q->datai[i]    = data + q->mid;
  }
  free(bitmap);
  mibitstream_free(s);
  free(buf);
}

void _plac_decompress_block(f32 *block, void *_plac) {
  plac_decompress_t plac = _plac;
  if (plac->callback) plac->callback(block, block_len, plac->userdata);
}

bool plac_decompress_block(plac_decompress_t plac) {
  if (plac->stream->pos == plac->stream->size) return false;
  plac_read_data(plac, &plac->q);
  dequantize(&plac->q);
  mulaw_expand(plac->q.dataf, block_len);
  mdctf_put(plac->mdct, plac->q.dataf, block_len);
  return true;
}
