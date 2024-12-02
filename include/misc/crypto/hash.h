#pragma once
#include <libc-base.h>

__attr(pure) u16 CRC16(const void *_data, size_t size) __THROW __attr_readonly(1, 2);
__attr(pure) u32 CRC32(const void *_data, size_t size) __THROW __attr_readonly(1, 2);

typedef struct _MD2_t {
  byte b[16];
  byte p;
  byte c[16]; // cksm 不知道啥用，反正计算要用到
  byte h[16]; // hash值
} MD2_t;

void MD2_init(MD2_t *_rest m) __THROW __nnull(1);
void MD2_input(MD2_t *_rest m, const void *_rest data, size_t len) __THROW __nnull(1, 2)
    __attr_readonly(2, 3);
const void *MD2_end(MD2_t *_rest m) __THROW __nnull(1);
const void *MD2_val(const MD2_t *_rest m) __THROW __nnull(1);
void        MD2_print(const MD2_t *_rest m) __THROW __nnull(1);
const void *MD2(MD2_t *_rest m, const void *_rest data, size_t len) __THROW __nnull(1, 2)
    __attr_readonly(2, 3);

typedef struct _MD5_t {
  byte b[64]; // 输入缓冲区
  byte p;     // 输入缓冲区指针
  u64  s;     // 大小
  u32  h[4];  // hash值
} MD5_t;

void MD5_init(MD5_t *_rest m) __THROW __nnull(1);
void MD5_input(MD5_t *_rest m, const void *_rest data, size_t len) __THROW __nnull(1, 2)
    __attr_readonly(2, 3);
const void *MD5_end(MD5_t *_rest m) __THROW __nnull(1);
const void *MD5_val(const MD5_t *_rest m) __THROW __nnull(1);
void        MD5_print(const MD5_t *_rest m) __THROW __nnull(1);
const void *MD5(MD5_t *_rest m, const void *_rest data, size_t len) __THROW __nnull(1, 2)
    __attr_readonly(2, 3);

typedef struct _SHA256_t {
  byte b[64]; // 输入缓冲区
  byte p;     // 输入缓冲区指针
  u64  s;     // 大小
  u32  h[8];  // hash值
} SHA256_t;

void SHA256_init(SHA256_t *_rest s) __THROW __nnull(1);
void SHA256_input(SHA256_t *_rest s, const void *_rest data, size_t len) __THROW __nnull(1, 2)
    __attr_readonly(2, 3);
const void *SHA256_end(SHA256_t *_rest s) __THROW __nnull(1);
const void *SHA256_val(const SHA256_t *_rest s) __THROW __nnull(1);
void        SHA256_print(const SHA256_t *_rest s) __THROW __nnull(1);
const void *SHA256(SHA256_t *_rest s, const void *_rest data, size_t len) __THROW __nnull(1, 2)
    __attr_readonly(2, 3);
