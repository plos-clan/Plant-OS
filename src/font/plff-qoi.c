#include <font.h>

#define QOI_OP_INDEX 0x00 /* 00xxxxxx */
#define QOI_OP_DIFF  0x40 /* 01xxxxxx */
#define QOI_OP_LUMA  0x80 /* 10xxxxxx */
#define QOI_OP_RUN   0xc0 /* 11xxxxxx */
#define QOI_OP_RGB   0xfe /* 11111110 */
#define QOI_OP_RGBA  0xff /* 11111111 */
#define QOI_MASK     0xc0 /* 11000000 */

finline int _color_hash(color_t c) {
  return (c.r * 3 + c.g * 5 + c.b * 7 + c.a * 11) % 64;
}

dlhidden void *plff_encode_rgb(const void *_data, size_t img_size, size_t *size_p) {
  if (_data == null || img_size == 0 || size_p == null) return null;
  const byte *data     = _data;
  const byte *data_end = _data + img_size * 3;

  byte *mem = malloc(img_size * 4);
  byte *d   = mem;
  if (!mem) return null;

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

  return realloc(mem, *size_p = d - mem);
}

dlhidden void *plff_encode_rgba(const void *_data, size_t img_size, size_t *size_p) {
  if (_data == null || img_size == 0 || size_p == null) return null;
  const byte *data     = _data;
  const byte *data_end = _data + img_size * 4;

  int max_size = img_size * 5;

  byte *mem = malloc(max_size);
  byte *d   = mem;
  if (!mem) return null;

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

  return realloc(mem, *size_p = d - mem);
}

dlhidden void *plff_decode_rgb(const void *data, size_t size, size_t img_size) {
  const byte *s        = data;
  const byte *data_end = data + size;

  byte *mem     = malloc(img_size * 3);
  byte *mem_end = mem + img_size * 3;
  byte *d       = mem;
  if (!mem) return null;

  int     run       = 0;
  color_t index[64] = {};
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

    if ((b1 & QOI_MASK) == QOI_OP_INDEX) {
      px = index[b1];
      continue;
    }

    if (b1 == QOI_OP_RGB) {
      px.r = *s++;
      px.g = *s++;
      px.b = *s++;
      continue;
    }

    if ((b1 & QOI_MASK) == QOI_OP_DIFF) {
      px.r += ((b1 >> 4) & 0x03) - 2;
      px.g += ((b1 >> 2) & 0x03) - 2;
      px.b += (b1 & 0x03) - 2;
      continue;
    }

    if ((b1 & QOI_MASK) == QOI_OP_LUMA) {
      int b2  = *s++;
      int vg  = (b1 & 0x3f) - 32;
      px.r   += vg - 8 + ((b2 >> 4) & 0x0f);
      px.g   += vg;
      px.b   += vg - 8 + (b2 & 0x0f);
      continue;
    }

    if ((b1 & QOI_MASK) == QOI_OP_RUN) {
      run = (b1 & 0x3f);
      continue;
    }
  }

  return mem;
}

dlhidden void *plff_decode_rgba(const void *data, size_t size, size_t img_size) {
  const byte *s        = data;
  const byte *data_end = data + size;

  byte *mem     = malloc(img_size * 4);
  byte *mem_end = mem + img_size * 4;
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

    if ((b1 & QOI_MASK) == QOI_OP_INDEX) {
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

    if ((b1 & QOI_MASK) == QOI_OP_DIFF) {
      px.r += ((b1 >> 4) & 0x03) - 2;
      px.g += ((b1 >> 2) & 0x03) - 2;
      px.b += (b1 & 0x03) - 2;
      continue;
    }

    if ((b1 & QOI_MASK) == QOI_OP_LUMA) {
      int b2  = *s++;
      int vg  = (b1 & 0x3f) - 32;
      px.r   += vg - 8 + ((b2 >> 4) & 0x0f);
      px.g   += vg;
      px.b   += vg - 8 + (b2 & 0x0f);
      continue;
    }

    if ((b1 & QOI_MASK) == QOI_OP_RUN) {
      run = (b1 & 0x3f);
      continue;
    }
  }

  return mem;
}
