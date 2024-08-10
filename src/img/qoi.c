#include <img.h>

#define PADDING 0
#define MODDING 0

#define QOI_OP_INDEX    0x00 /* 00xxxxxx */
#define QOI_OP_DIFF     0x40 /* 01xxxxxx */
#define QOI_OP_LUMA     0x80 /* 10xxxxxx */
#define QOI_OP_RUN      0xc0 /* 11xxxxxx */
#define QOI_OP_RGB      0xfe /* 11111110 */
#define QOI_OP_RGBA     0xff /* 11111111 */
#define QOI_MASK_2      0xc0 /* 11000000 */
#define QOI_MAGIC       0x66696f71
#define QOI_HEADER_SIZE 14

finline int _color_hash(color_t c) {
  return (c.r * 3 + c.g * 5 + c.b * 7 + c.a * 11) % 64;
}

#if PADDING
static const uint8_t qoi_padding[8] = {0, 0, 0, 0, 0, 0, 0, 1};
#endif

dlexport void *qoi_encode_rgb(const void *_data, int width, int height, size_t *size) {
  if (_data == null || width <= 0 || height <= 0 || size == null) return null;
  const byte *data     = _data;
  const byte *data_end = _data + width * height * 3;

#if PADDING
  int max_size = width * height * 4 + QOI_HEADER_SIZE + sizeof(qoi_padding);
#else
  int max_size = width * height * 4 + QOI_HEADER_SIZE;
#endif

  byte *mem = malloc(max_size);
  byte *d   = mem;
  if (!mem) return null;

  *(u32 *)d  = QOI_MAGIC;
  d         += 4;
  *(u32 *)d  = big_endian(width);
  d         += 4;
  *(u32 *)d  = big_endian(height);
  d         += 4;
  *d++       = 3;
  *d++       = 1;

  color_t index[64] = {};
  int     run       = 0;
  color_t px        = {0, 0, 0, 255};
  for (color_t px_prev = px; data < data_end; px_prev = px) {
    px.r = *data++;
    px.g = *data++;
    px.b = *data++;

    if (px.v == px_prev.v) {
      if (++run == 62) {
        *d++ = QOI_OP_RUN | (run - 1);
        run  = 0;
      }
      continue;
    }

    if (run > 0) {
      *d++ = QOI_OP_RUN | (run - 1);
      run  = 0;
    }

    int index_pos = _color_hash(px);
    if (index[index_pos].v == px.v) {
      *d++ = QOI_OP_INDEX | index_pos;
      continue;
    }

    index[index_pos] = px;

    i8 vr = px.r - px_prev.r;
    i8 vg = px.g - px_prev.g;
    i8 vb = px.b - px_prev.b;

    i8 vg_r = vr - vg;
    i8 vg_b = vb - vg;

    if (vr > -3 && vr < 2 && vg > -3 && vg < 2 && vb > -3 && vb < 2) {
      *d++ = QOI_OP_DIFF | (vr + 2) << 4 | (vg + 2) << 2 | (vb + 2);
      continue;
    }

    if (vg_r > -9 && vg_r < 8 && vg > -33 && vg < 32 && vg_b > -9 && vg_b < 8) {
      *d++ = QOI_OP_LUMA | (vg + 32);
      *d++ = (vg_r + 8) << 4 | (vg_b + 8);
      continue;
    }

    *d++ = QOI_OP_RGB;
    *d++ = px.r;
    *d++ = px.g;
    *d++ = px.b;
  }

  if (run > 0) {
    *d++ = QOI_OP_RUN | (run - 1);
    run  = 0;
  }

#if PADDING
  for (int i = 0; i < sizeof(qoi_padding); i++)
    *d++ = qoi_padding[i];
#endif

  return realloc(mem, *size = d - mem);
}

dlexport void *qoi_encode_rgba(const void *_data, int width, int height, size_t *size) {
  if (_data == null || width <= 0 || height <= 0 || size == null) return null;
  const byte *data     = _data;
  const byte *data_end = _data + width * height * 4;

#if PADDING
  int max_size = width * height * 5 + QOI_HEADER_SIZE + sizeof(qoi_padding);
#else
  int max_size = width * height * 5 + QOI_HEADER_SIZE;
#endif

  byte *mem = malloc(max_size);
  byte *d   = mem;
  if (!mem) return null;

  *(u32 *)d  = QOI_MAGIC;
  d         += 4;
  *(u32 *)d  = big_endian(width);
  d         += 4;
  *(u32 *)d  = big_endian(height);
  d         += 4;
  *d++       = 4;
  *d++       = 1;

  color_t index[64] = {};
  int     run       = 0;
  color_t px        = {0, 0, 0, 255};
  for (color_t px_prev = px; data < data_end; px_prev = px) {
    px.r = *data++;
    px.g = *data++;
    px.b = *data++;
    px.a = *data++;

    if (px.v == px_prev.v) {
      run++;
      if (run == 62) {
        *d++ = QOI_OP_RUN | (run - 1);
        run  = 0;
      }
      continue;
    }

    if (run > 0) {
      *d++ = QOI_OP_RUN | (run - 1);
      run  = 0;
    }

    int index_pos = _color_hash(px);
    if (index[index_pos].v == px.v) {
      *d++ = QOI_OP_INDEX | index_pos;
      continue;
    }

    index[index_pos] = px;

    if (px.a != px_prev.a) {
      *d++ = QOI_OP_RGBA;
      *d++ = px.r;
      *d++ = px.g;
      *d++ = px.b;
      *d++ = px.a;
      continue;
    }

    i8 vr = px.r - px_prev.r;
    i8 vg = px.g - px_prev.g;
    i8 vb = px.b - px_prev.b;

    i8 vg_r = vr - vg;
    i8 vg_b = vb - vg;

    if (vr > -3 && vr < 2 && vg > -3 && vg < 2 && vb > -3 && vb < 2) {
      *d++ = QOI_OP_DIFF | (vr + 2) << 4 | (vg + 2) << 2 | (vb + 2);
      continue;
    }

    if (vg_r > -9 && vg_r < 8 && vg > -33 && vg < 32 && vg_b > -9 && vg_b < 8) {
      *d++ = QOI_OP_LUMA | (vg + 32);
      *d++ = (vg_r + 8) << 4 | (vg_b + 8);
      continue;
    }

    *d++ = QOI_OP_RGB;
    *d++ = px.r;
    *d++ = px.g;
    *d++ = px.b;
  }

  if (run > 0) {
    *d++ = QOI_OP_RUN | (run - 1);
    run  = 0;
  }

#if PADDING
  for (int i = 0; i < sizeof(qoi_padding); i++)
    *d++ = qoi_padding[i];
#endif

  return realloc(mem, *size = d - mem);
}

dlexport void *qoi_decode_rgb(const void *data, size_t size, int width, int height) {
  const byte *s        = data;
  const byte *data_end = data + size;

  byte *mem     = malloc(width * height * 3);
  byte *mem_end = mem + width * height * 3;
  byte *d       = mem;
  if (!mem) return null;

  int     run       = 0;
  color_t index[64] = {};
  index[53]         = (color_t){0, 0, 0, 255};
  color_t px        = {0, 0, 0, 255};
  for (; d < mem_end; *d++ = px.r, *d++ = px.g, *d++ = px.b) {
    if (run > 0) {
      run--;
      continue;
    }

    if (s == data_end) {
      free(mem);
      return null;
    }
    int b1 = *s++;

    index[_color_hash(px)] = px;

    if ((b1 & QOI_MASK_2) == QOI_OP_INDEX) {
      px = index[b1];
      continue;
    }

    if (b1 == QOI_OP_RGB) {
      px.r = *s++;
      px.g = *s++;
      px.b = *s++;
      continue;
    }

    if ((b1 & QOI_MASK_2) == QOI_OP_DIFF) {
      px.r += ((b1 >> 4) & 0x03) - 2;
      px.g += ((b1 >> 2) & 0x03) - 2;
      px.b += (b1 & 0x03) - 2;
      continue;
    }

    if ((b1 & QOI_MASK_2) == QOI_OP_LUMA) {
      int b2  = *s++;
      int vg  = (b1 & 0x3f) - 32;
      px.r   += vg - 8 + ((b2 >> 4) & 0x0f);
      px.g   += vg;
      px.b   += vg - 8 + (b2 & 0x0f);
      continue;
    }

    if ((b1 & QOI_MASK_2) == QOI_OP_RUN) {
      run = (b1 & 0x3f);
      continue;
    }
  }

  return mem;
}

dlexport void *qoi_decode_rgba(const void *data, size_t size, int width, int height) {
  const byte *s        = data;
  const byte *data_end = data + size;

  byte *mem     = malloc(width * height * 4);
  byte *mem_end = mem + width * height * 4;
  byte *d       = mem;
  if (!mem) return null;

  int     run       = 0;
  color_t index[64] = {};
  color_t px        = {0, 0, 0, 255};
  for (; d < mem_end; *d++ = px.r, *d++ = px.g, *d++ = px.b, *d++ = px.a) {
    if (run > 0) {
      run--;
      continue;
    }

    if (s == data_end) {
      free(mem);
      return null;
    }
    int b1 = *s++;

    index[_color_hash(px)] = px;

    if ((b1 & QOI_MASK_2) == QOI_OP_INDEX) {
      px = index[b1];
      continue;
    }

    if (b1 == QOI_OP_RGB) {
      px.r = *s++;
      px.g = *s++;
      px.b = *s++;
      continue;
    }

    if (b1 == QOI_OP_RGBA) {
      px.r = *s++;
      px.g = *s++;
      px.b = *s++;
      px.a = *s++;
      continue;
    }

    if ((b1 & QOI_MASK_2) == QOI_OP_DIFF) {
      px.r += ((b1 >> 4) & 0x03) - 2;
      px.g += ((b1 >> 2) & 0x03) - 2;
      px.b += (b1 & 0x03) - 2;
      continue;
    }

    if ((b1 & QOI_MASK_2) == QOI_OP_LUMA) {
      int b2  = *s++;
      int vg  = (b1 & 0x3f) - 32;
      px.r   += vg - 8 + ((b2 >> 4) & 0x0f);
      px.g   += vg;
      px.b   += vg - 8 + (b2 & 0x0f);
      continue;
    }

    if ((b1 & QOI_MASK_2) == QOI_OP_RUN) {
      run = (b1 & 0x3f);
      continue;
    }
  }

  return mem;
}

dlexport void *qoi_encode(const void *data, size_t *size, int width, int height, int channels) {
  if (data == null || width <= 0 || height <= 0 || channels < 3 || channels > 4 || size == null) {
    return null;
  }

#if MODDING
  const byte *_d = data;
  byte       *d  = malloc(width * height * channels);
  if (d == null) return null;
  memcpy(d, data, width * height * channels);

  for (int x = channels; x < width * channels; x += channels) {
    d[x]     = _d[x] - _d[x - channels] + 128;
    d[x + 1] = _d[x + 1] - _d[x - channels + 1] + 128;
    d[x + 2] = _d[x + 2] - _d[x - channels + 2] + 128;
  }
  for (int y = 1; y < height; y++) {
    int _y  = y * width * channels;
    int __y = (y - 1) * width * channels;
    for (int x = 0; x < width * channels; x += channels) {
      byte       *dst = d + _y + x;
      byte       *cur = d + _y + x;
      const byte *top = _d + __y + x;
      dst[0]          = cur[0] - top[0] + 128;
      dst[1]          = cur[1] - top[1] + 128;
      dst[2]          = cur[2] - top[2] + 128;
    }
  }
#else
  const byte *d = data;
#endif

  void *ptr = null;
  if (channels == 3) {
    ptr = qoi_encode_rgb(d, width, height, size);
  } else if (channels == 4) {
    ptr = qoi_encode_rgba(d, width, height, size);
  }

#if MODDING
  free(d);
#endif

  return ptr;
}

dlexport void *qoi_decode(const void *data, size_t size, int *width, int *height, int *channels) {
  if (data == null || size <= QOI_HEADER_SIZE || width == null || height == null ||
      channels == null)
    return null;
  const byte *s = data;

#if PADDING
  size -= QOI_HEADER_SIZE + sizeof(qoi_padding);
#else
  size -= QOI_HEADER_SIZE;
#endif

  if (*(u32 *)s != QOI_MAGIC) return null;
  s         += 4;
  *width     = big_endian(*(u32 *)s);
  s         += 4;
  *height    = big_endian(*(u32 *)s);
  s         += 4;
  *channels  = *s++;
  if (*s++ != 1) return null;

  if (*width <= 0 || *height <= 0) return null;

  byte *d = null;
  if (*channels == 3) d = qoi_decode_rgb(s, size, *width, *height);
  if (*channels == 4) d = qoi_decode_rgba(s, size, *width, *height);
  if (d == null) return null;

#if MODDING
  if (*channels == 3) {
    for (int x = 3; x < *width * 3; x += 3) {
      d[x]     = d[x] + d[x - 3] - 128;
      d[x + 1] = d[x + 1] + d[x - 2] - 128;
      d[x + 2] = d[x + 2] + d[x - 1] - 128;
    }
    for (int y = 1; y < *height; y++) {
      int _y  = y * *width * 3;
      int __y = (y - 1) * *width * 3;
      for (int x = 0; x < *width * 3; x += 3) {
        byte *cur = d + _y + x;
        byte *top = d + __y + x;
        cur[0]    = cur[0] + top[0] - 128;
        cur[1]    = cur[1] + top[1] - 128;
        cur[2]    = cur[2] + top[2] - 128;
      }
    }
  } else {
    for (int x = 4; x < *width * 4; x += 4) {
      d[x]     = d[x] + d[x - 4] - 128;
      d[x + 1] = d[x + 1] + d[x - 3] - 128;
      d[x + 2] = d[x + 2] + d[x - 2] - 128;
      d[x + 3] = d[x + 3] + d[x - 1] - 128;
    }
    for (int y = 1; y < *height; y++) {
      int _y  = y * *width * 4;
      int __y = (y - 1) * *width * 4;
      for (int x = 0; x < *width * 4; x += 4) {
        byte *cur = d + _y + x;
        byte *top = d + __y + x;
        cur[0]    = cur[0] + top[0] - 128;
        cur[1]    = cur[1] + top[1] - 128;
        cur[2]    = cur[2] + top[2] - 128;
        cur[3]    = cur[3] + top[3] - 128;
      }
    }
  }
#endif

  return d;
}

dlexport void *qoi_decode_f32(const void *data, size_t size, int *width, int *height,
                              int *channels) {
  byte *img = qoi_decode(data, size, width, height, channels);
  if (img == null) return null;

  size = *width * *height * *channels;

  float *d = malloc(size * sizeof(float));
  if (d == null) {
    free(img);
    return null;
  }

  for (int i = 0; i < size; i++) {
    d[i] = img[i] / 255.f;
  }

  free(img);

  return d;
}

// dlexport void *qoi_load(const char *filename, int *width, int *height, int *channels) {
//   if (filename == null || width == null || height == null || channels == null) return null;

//   size_t size;
//   void  *data = map_file(filename, &size, O_RDONLY);
//   if (data == null) return null;

//   void *img = qoi_decode(data, size, width, height, channels);

//   unmap_file(data, size);

//   return img;
// }

// dlexport int qoi_save(const char *filename, const void *data, int width, int height, int channels) {
//   if (filename == null || data == null || width <= 0 || height <= 0 || channels < 3 || channels > 4)
//     return -1;

//   size_t size;
//   void  *bytes = qoi_encode(data, &size, width, height, channels);
//   if (bytes == null) return -1;

//   int rets = write_to_file(filename, bytes, size);

//   free(bytes);

//   return rets;
// }
