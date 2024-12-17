#pragma once
#include "../base.h"

#pragma GCC system_header

#ifdef ALL_IMPLEMENTATION
#  define MSTREAM_IMPLEMENTATION
#endif

#define MSTREAM_IMPLEMENTATION

/**
 *\struct mistream
 *\brief 内存读取流结构
 */
typedef struct mistream {
  const void *buf;
  size_t      size;
  size_t      pos;
} *mistream_t;

/**
 *\struct mostream
 *\brief 内存写入流结构
 */
typedef struct mostream {
  void  *buf;
  size_t size;
  size_t capacity;
} *mostream_t;

// typedef struct mstream {
//   void  *buf;
//   size_t blksize;
//   size_t size;
// } *mstream_t;

#ifdef MSTREAM_IMPLEMENTATION
#  define extern static
#endif

/**
 *\brief 创建一个新的内存读取流
 *\param[in] buffer 数据缓冲区
 *\param[in] size 缓冲区大小
 *\return 新的内存读取流指针
 */
extern mistream_t mistream_alloc(const void *buffer, size_t size);

/**
 *\brief 释放内存读取流
 *\param[in] stream 内存读取流指针
 */
extern void mistream_free(mistream_t stream);

/**
 *\brief 从内存读取流中读取数据
 *\param[in] stream 内存读取流指针
 *\param[out] data 存储读取数据的缓冲区
 *\param[in] size 要读取的数据大小
 *\return 读取的字节数
 */
extern size_t mistream_read(mistream_t stream, void *data, size_t size);

/**
 *\brief 创建一个新的内存写入流
 *\param[in] capacity 初始容量
 *\return 新的内存写入流指针
 */
extern mostream_t mostream_alloc(size_t capacity);

/**
 *\brief 释放内存写入流
 *\param[in] stream 内存写入流指针
 */
extern void mostream_free(mostream_t stream);

/**
 *\brief 向内存写入流中写入数据
 *\param[in] stream 内存写入流指针
 *\param[in] data 要写入的数据
 *\param[in] size 数据大小
 *\return 写入的字节数
 */
extern size_t mostream_write(mostream_t stream, const void *data, size_t size);

#ifdef MSTREAM_IMPLEMENTATION
#  undef extern
#endif

#ifdef MSTREAM_IMPLEMENTATION

static mistream_t mistream_alloc(const void *buffer, size_t size) {
  mistream_t stream = malloc(sizeof(struct mistream));
  if (stream == null) return null;
  stream->buf  = buffer;
  stream->size = size;
  stream->pos  = 0;
  return stream;
}

static void mistream_free(mistream_t stream) {
  if (stream == null) return;
  free(stream);
}

static size_t mistream_read(mistream_t stream, void *data, size_t size) {
  if (stream->pos + size > stream->size) size = stream->size - stream->pos;
  memcpy(data, stream->buf + stream->pos, size);
  stream->pos += size;
  return size;
}

static int mistream_byte(mistream_t stream) {
  byte data;
  return mistream_read(stream, &data, 1) == 1 ? data : -1;
}

static mostream_t mostream_alloc(size_t capacity) {
  mostream_t stream = malloc(sizeof(struct mostream));
  if (stream == null) return null;
  stream->buf      = malloc(capacity);
  stream->size     = 0;
  stream->capacity = capacity;
  return stream;
}

static void mostream_free(mostream_t stream) {
  if (stream == null) return;
  free(stream->buf);
  free(stream);
}

static size_t mostream_write(mostream_t stream, const void *data, size_t size) {
  if (stream->size + size > stream->capacity) {
    size_t new_capacity = stream->capacity * 2;
    while (new_capacity < stream->size + size) {
      new_capacity *= 2;
    }
    char *new_buffer = realloc(stream->buf, new_capacity);
    if (new_buffer == null) return 0;
    stream->buf      = new_buffer;
    stream->capacity = new_capacity;
  }
  memcpy(stream->buf + stream->size, data, size);
  stream->size += size;
  return size;
}

static bool mostream_byte(mostream_t stream, byte data) {
  return mostream_write(stream, &data, 1) == 1;
}

#endif
