#pragma once
#include "../base.h"

#pragma GCC system_header

#ifdef ALL_IMPLEMENTATION
#  define CIRBUF_IMPLEMENTATION
#endif

// $$$$$ ========== ========== ========== ========== ========== ========== ========== ========== $$$$$

#if !defined(COMPILE_DATA_STRUCTURE_LIBRARY) && defined(CIRBUF_IMPLEMENTATION)
#  define API static
#else
#  define API dlimport
#endif

/**
 *\struct Circular buffer
 *\brief 循环缓冲区
 */
typedef struct cirbuf {
  byte  *buf;
  size_t size;
  size_t head; /**< 指向下一个读取的位置 */
  size_t tail; /**< 指向下一个写入的位置 */
  size_t len;  /**< 当前的元素个数 */
} *cirbuf_t;

/**
 *\brief 创建一个新的循环缓冲区
 *\param[in] size 循环缓冲区最大大小
 *\return 新的循环缓冲区指针
 */
API cirbuf_t cirbuf_alloc(size_t size);

/**
 *\brief 销毁循环缓冲区并释放内存
 *\param[in] cirbuf 循环缓冲区指针
 */
API void cirbuf_free(cirbuf_t cirbuf);

/**
 *\brief 将字节插入缓冲区
 *\param[in] cirbuf 缓冲区指针
 *\param[in] data 插入的元素
 */
API bool cirbuf_put(cirbuf_t cirbuf, byte data);

/**
 *\brief 将元素移出缓冲区
 *\param[in] cirbuf 缓冲区指针
 *\return 移出的元素，失败返回 0
 */
API byte cirbuf_get(cirbuf_t cirbuf);

/**
 *\brief 将元素移出缓冲区
 *\param[in] cirbuf 缓冲区指针
 *\param[out] data_p 移出的元素指针
 *\return 是否成功
 */
API bool cirbuf_pget(cirbuf_t cirbuf, byte *data_p);

/**
 *\brief 判断缓冲区是否为空
 *\param[in] cirbuf 缓冲区指针
 *\return 若缓冲区为空，则返回true，否则返回false
 */
API bool cirbuf_isempty(cirbuf_t cirbuf);

/**
 *\brief 判断缓冲区是否为满
 *\param[in] cirbuf 缓冲区指针
 *\return 若缓冲区为满，则返回true，否则返回false
 */
API bool cirbuf_isfull(cirbuf_t cirbuf);

/**
 *\brief 获取循环缓冲区的最大大小
 *\param[in] cirbuf 循环缓冲区指针
 *\return 循环缓冲区的最大大小
 */
API size_t cirbuf_size(cirbuf_t cirbuf);

/**
 *\brief 获取循环缓冲区的元素个数
 *\param[in] cirbuf 循环缓冲区指针
 *\return 循环缓冲区的元素个数
 */
API size_t cirbuf_len(cirbuf_t cirbuf);

#undef API

// $$$$$ ========== ========== ========== ========== ========== ========== ========== ========== $$$$$

#ifdef COMPILE_DATA_STRUCTURE_LIBRARY
#  define API dlexport
#else
#  define API static
#endif

#if defined(COMPILE_DATA_STRUCTURE_LIBRARY) || defined(CIRBUF_IMPLEMENTATION)

API cirbuf_t cirbuf_alloc(size_t size) {
  cirbuf_t cirbuf = malloc(sizeof(*cirbuf));
  if (cirbuf == null) return null;
  cirbuf->buf = malloc(size);
  if (cirbuf->buf == null) {
    free(cirbuf);
    return null;
  }
  cirbuf->size = size;
  cirbuf->head = 0;
  cirbuf->tail = 0;
  cirbuf->len  = 0;
  return cirbuf;
}

API void cirbuf_free(cirbuf_t cirbuf) {
  if (cirbuf == null) return;
  free(cirbuf->buf);
  free(cirbuf);
}

API bool cirbuf_put(cirbuf_t cirbuf, byte data) {
  if (cirbuf == null) return false;
  if (cirbuf->len == cirbuf->size) return false;
  cirbuf->buf[cirbuf->tail++] = data;
  if (cirbuf->tail == cirbuf->size) cirbuf->tail = 0;
  cirbuf->len++;
  return true;
}

API byte cirbuf_get(cirbuf_t cirbuf) {
  if (cirbuf == null) return 0;
  if (cirbuf->len == 0) return 0;
  byte data = cirbuf->buf[cirbuf->head++];
  if (cirbuf->head == cirbuf->size) cirbuf->head = 0;
  cirbuf->len--;
  return data;
}

API bool cirbuf_pget(cirbuf_t cirbuf, byte *data_p) {
  if (cirbuf == null) return false;
  if (cirbuf->len == 0) return false;
  byte data = cirbuf->buf[cirbuf->head++];
  if (data_p) *data_p = data;
  if (cirbuf->head == cirbuf->size) cirbuf->head = 0;
  cirbuf->len--;
  return true;
}

API bool cirbuf_isempty(cirbuf_t cirbuf) {
  return cirbuf == null || cirbuf->len == 0;
}

API bool cirbuf_isfull(cirbuf_t cirbuf) {
  return cirbuf != null && cirbuf->len == cirbuf->size;
}

API size_t cirbuf_size(cirbuf_t cirbuf) {
  return (cirbuf == null) ? 0 : cirbuf->size;
}

API size_t cirbuf_len(cirbuf_t cirbuf) {
  return (cirbuf == null) ? 0 : cirbuf->len;
}

#endif

#undef API
