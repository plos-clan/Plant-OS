#pragma once
#include "../base.h"

#pragma GCC system_header

#ifdef ALL_IMPLEMENTATION
#  define MBITSTREAM_IMPLEMENTATION
#endif

#define MBITSTREAM_IMPLEMENTATION

/**
 *\struct mibitstream
 *\brief 内存位读取流结构
 */
typedef struct mibitstream {
  const void *buf;
  size_t      size;
  size_t      pos;
  size_t      bit_pos;
} *mibitstream_t;

/**
 *\struct mobitstream
 *\brief 内存位写入流结构
 */
typedef struct mobitstream {
  void  *buf;
  size_t size;
  size_t capacity;
  size_t bit_pos;
} *mobitstream_t;

#ifdef MBITSTREAM_IMPLEMENTATION
#  define extern static
#endif

/**
 *\brief 创建一个新的内存位读取流
 *\param[in] buffer 数据缓冲区
 *\param[in] size 缓冲区大小
 *\return 新的内存位读取流指针
 */
extern mibitstream_t mibitstream_alloc(const void *buffer, size_t size);

/**
 *\brief 释放内存位读取流
 *\param[in] stream 内存位读取流指针
 */
extern void mibitstream_free(mibitstream_t stream);

/**
 *\brief 从内存位读取流中读取一个位
 *\param[in] stream 内存位读取流指针
 *\return 读取的位（0 或 1），如果读取失败返回 -1
 */
extern int mibitstream_read_bit(mibitstream_t stream);

/**
 *\brief 从内存位读取流中读取指定位数的位
 *\param[in] stream 内存位读取流指针
 *\param[in] nbits 要读取的位数
 *\return 读取的位数，如果读取失败返回 0
 */
extern size_t mibitstream_read_bits(mibitstream_t stream, size_t nbits);

/**
 *\brief 从内存位读取流中读取指定位数的位（有符号整数版本）
 *\param[in] stream 内存位读取流指针
 *\param[in] nbits 要读取的位数
 *\return 读取的位数，如果读取失败返回 0
 */
extern ssize_t mibitstream_read_bitsi(mibitstream_t stream, size_t nbits);

/**
 *\brief 创建一个新的内存位写入流
 *\param[in] capacity 初始容量
 *\return 新的内存位写入流指针
 */
extern mobitstream_t mobitstream_alloc(size_t capacity);

/**
 *\brief 释放内存位写入流
 *\param[in] stream 内存位写入流指针
 */
extern void mobitstream_free(mobitstream_t stream);

/**
 *\brief 向内存位写入流中写入一个位
 *\param[in] stream 内存位写入流指针
 *\param[in] bit 要写入的位（0 或 1）
 *\return 写入的位数，如果写入失败返回 0
 */
extern size_t mobitstream_write_bit(mobitstream_t stream, bool bit);

/**
 *\brief 向内存位写入流中写入指定位数的位
 *
 *\param[in] bits 要写入的数据 (从低位开始)
 *\param[in] nbits 要写入的位数
 *\return 写入的位数，如果写入失败返回 0
 */
extern size_t mobitstream_write_bits(mobitstream_t stream, size_t bits, size_t nbits);

#ifdef MBITSTREAM_IMPLEMENTATION
#  undef extern
#endif

#ifdef MBITSTREAM_IMPLEMENTATION

static mibitstream_t mibitstream_alloc(const void *buffer, size_t size) {
  mibitstream_t stream = malloc(sizeof(struct mibitstream));
  if (stream == NULL) return NULL;
  stream->buf     = buffer;
  stream->size    = size;
  stream->pos     = 0;
  stream->bit_pos = 0;
  return stream;
}

static void mibitstream_free(mibitstream_t stream) {
  if (stream == NULL) return;
  free(stream);
}

static int mibitstream_read_bit(mibitstream_t stream) {
  if (stream->pos >= stream->size) return -1;
  uint8_t byte = ((uint8_t *)stream->buf)[stream->pos];
  int     bit  = (byte >> (7 - stream->bit_pos)) & 1;
  stream->bit_pos++;
  if (stream->bit_pos == 8) {
    stream->bit_pos = 0;
    stream->pos++;
  }
  return bit;
}

static size_t mibitstream_read_bits(mibitstream_t stream, size_t nbits) {
  size_t bits = 0;
  for (size_t i = 0; i < nbits; i++) {
    int bit = mibitstream_read_bit(stream);
    if (bit == -1) return 0;
    bits |= (bit << i);
  }
  return bits;
}

static ssize_t mibitstream_read_bitsi(mibitstream_t stream, size_t nbits) {
  ssize_t bits = 0;
  for (size_t i = 0; i < nbits - 1; i++) {
    int bit = mibitstream_read_bit(stream);
    if (bit == -1) return 0;
    bits |= (bit << i);
  }
  int sign = mibitstream_read_bit(stream);
  if (sign == -1) return 0;
  return sign ? (ssize_t) ~(((size_t)1 << (nbits - 1)) - 1) | bits : bits;
}

static mobitstream_t mobitstream_alloc(size_t capacity) {
  mobitstream_t stream = malloc(sizeof(struct mobitstream));
  if (stream == NULL) return NULL;
  stream->buf      = malloc(capacity);
  stream->size     = 0;
  stream->capacity = capacity;
  stream->bit_pos  = 0;
  return stream;
}

static void mobitstream_free(mobitstream_t stream) {
  if (stream == NULL) return;
  free(stream->buf);
  free(stream);
}

static size_t mobitstream_write_bit(mobitstream_t stream, bool bit) {
  if (stream->size * 8 + stream->bit_pos >= stream->capacity * 8) {
    size_t new_capacity = stream->capacity * 2;
    while (new_capacity * 8 < stream->size * 8 + stream->bit_pos + 1) {
      new_capacity *= 2;
    }
    uint8_t *new_buffer = realloc(stream->buf, new_capacity);
    if (new_buffer == NULL) return 0;
    stream->buf      = new_buffer;
    stream->capacity = new_capacity;
  }
  if (bit) {
    ((uint8_t *)stream->buf)[stream->size] |= (1 << (7 - stream->bit_pos));
  } else {
    ((uint8_t *)stream->buf)[stream->size] &= ~(1 << (7 - stream->bit_pos));
  }
  stream->bit_pos++;
  if (stream->bit_pos == 8) {
    stream->bit_pos = 0;
    stream->size++;
  }
  return 1;
}

static size_t mobitstream_write_bits(mobitstream_t stream, size_t bits, size_t nbits) {
  size_t written = 0;
  for (size_t i = 0; i < nbits; i++) {
    size_t result = mobitstream_write_bit(stream, (bits >> i) & 1);
    if (result == 0) return written;
    written++;
  }
  return written;
}

#endif