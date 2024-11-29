#pragma once
#include <libc-base.h>

int base64_enc(char *_rest dst, size_t dstlen, const void *_rest src, size_t srclen)
    __THROW __nnull(1, 3) __attr_readonly(3, 4) __attr_writeonly(1);

int base64_dec(void *_rest dst, size_t dstlen, cstr _rest src, size_t srclen) __THROW __nnull(1, 3)
    __attr_readonly(3, 4) __attr_writeonly(1);

int hex_enc(char *_rest dst, size_t dstlen, const void *_rest src, size_t srclen)
    __THROW __nnull(1, 3) __attr_readonly(3, 4) __attr_writeonly(1);

int HEX_enc(char *_rest dst, size_t dstlen, const void *_rest src, size_t srclen)
    __THROW __nnull(1, 3) __attr_readonly(3, 4) __attr_writeonly(1);

int hex_dec(void *dst, cstr src) __THROW __nnull(1, 2) __attr_readonly(2) __attr_writeonly(1);

int hex_sndec(void *dst, cstr src, size_t srclen) __THROW __nnull(1, 2) __attr_readonly(2, 3)
    __attr_writeonly(1);

int hex_dndec(void *dst, size_t dstlen, cstr src) __THROW __nnull(1, 3) __attr_readonly(3)
    __attr_writeonly(1, 2);

int hex_ndec(void *dst, size_t dstlen, cstr src, size_t srclen) __THROW __nnull(1, 3)
    __attr_readonly(3, 4) __attr_writeonly(1, 2);

int caesar_enc(char *dst, cstr src, char passwd) __THROW __nnull(1, 2) __attr_readonly(2)
    __attr_writeonly(1);

int caesar_dec(char *dst, cstr src, char passwd) __THROW __nnull(1, 2) __attr_readonly(2)
    __attr_writeonly(1);
