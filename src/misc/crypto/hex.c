#include <misc.h>

static const char hex_lut[16]   = "0123456789abcdef";
static const char HEX_lut[16]   = "0123456789ABCDEF";
static const byte hex_rlut[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 1
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 2
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  -1, -1, -1, -1, -1, -1, // 3
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 4
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 5
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 6
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 7
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 8
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 9
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // a
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // b
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // c
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // d
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // e
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // f
};

// 十六进制编码 (小写)
// 参数错误，返回-1
// 目标缓冲区空间不足，返回 1
// 成功，返回 0
dlexport int hex_enc(char *_rest dst, size_t dstlen, const void *_rest src, size_t srclen) {
  if (!dst || !src) return -1;
  if (!srclen) return 0;

  size_t sizereq = srclen * 2;
  if (dstlen < sizereq) return 1;

  const byte *s = src;
  char       *d = dst;

  for (size_t i = 0; i < srclen; i++) {
    *(d++) = hex_lut[(s[i] >> 4) & 0x0f];
    *(d++) = hex_lut[s[i] & 0x0f];
  }

  if (dstlen > sizereq) *d = '\0';

  return 0;
}

// 十六进制编码 (大写)
// 参数错误，返回-1
// 目标缓冲区空间不足，返回 1
// 成功，返回 0
dlexport int HEX_enc(char *_rest dst, size_t dstlen, const void *_rest src, size_t srclen) {
  if (!dst || !src) return -1;
  if (!srclen) return 0;

  size_t sizereq = srclen * 2;
  if (dstlen < sizereq) return 1;

  const byte *s = src;
  char       *d = dst;

  for (size_t i = 0; i < srclen; i++) {
    *(d++) = HEX_lut[(s[i] >> 4) & 0x0f];
    *(d++) = HEX_lut[s[i] & 0x0f];
  }

  if (dstlen > sizereq) *d = '\0';

  return 0;
}

// 十六进制解码
dlexport int hex_dec(void *dst, cstr src) {
  if (!dst || !src) return -1;
#define dst  ((byte *)dst)
#define nsrc (src[srcoffset])
#define ndst (dst[dstoffset])

  int dstoffset = 0;
  for (int srcoffset = 0; nsrc; srcoffset++, dstoffset++) {
    if (nsrc < '0')
      break;
    else if (nsrc <= '9')
      ndst = nsrc - '0';
    else if (nsrc < 'A')
      break;
    else if (nsrc <= 'Z')
      ndst = nsrc - 'A' + 10;
    else if (nsrc < 'a')
      break;
    else if (nsrc <= 'z')
      ndst = nsrc - 'a' + 10;
    else
      break;
    ndst <<= 4;
    srcoffset++;
    if (nsrc < '0')
      break;
    else if (nsrc <= '9')
      ndst |= nsrc - '0';
    else if (nsrc < 'A')
      break;
    else if (nsrc <= 'Z')
      ndst |= nsrc - 'A' + 10;
    else if (nsrc < 'a')
      break;
    else if (nsrc <= 'z')
      ndst |= nsrc - 'a' + 10;
    else
      break;
  }

  return dstoffset;

#undef nsrc
#undef ndst
#undef dst
}
// 十六进制解码
dlexport int hex_sndec(void *dst, cstr src, size_t srclen) {
  if (!dst || !src) return -1;
  if (srclen == 0) return 0;
#define dst  ((byte *)dst)
#define nsrc (src[srcoffset])
#define ndst (dst[dstoffset])

  int dstoffset = 0;
  for (int srcoffset = 0; srcoffset < srclen; srcoffset++, dstoffset++) {
    if (nsrc < '0')
      break;
    else if (nsrc <= '9')
      ndst = nsrc - '0';
    else if (nsrc < 'A')
      break;
    else if (nsrc <= 'Z')
      ndst = nsrc - 'A' + 10;
    else if (nsrc < 'a')
      break;
    else if (nsrc <= 'z')
      ndst = nsrc - 'a' + 10;
    else
      break;
    ndst <<= 4;
    if (++srcoffset >= srclen) break;
    if (nsrc < '0')
      break;
    else if (nsrc <= '9')
      ndst |= nsrc - '0';
    else if (nsrc < 'A')
      break;
    else if (nsrc <= 'Z')
      ndst |= nsrc - 'A' + 10;
    else if (nsrc < 'a')
      break;
    else if (nsrc <= 'z')
      ndst |= nsrc - 'a' + 10;
    else
      break;
  }

  return dstoffset;

#undef nsrc
#undef ndst
#undef dst
}
// 十六进制解码
dlexport int hex_dndec(void *dst, size_t dstlen, cstr src) {
  if (!dst || !src) return -1;
  if (dstlen == 0) return 0;
#define dst  ((byte *)dst)
#define nsrc (src[srcoffset])
#define ndst (dst[dstoffset])

  int dstoffset = 0;
  for (int srcoffset = 0; nsrc && dstoffset < dstlen; srcoffset++, dstoffset++) {
    if (nsrc < '0')
      break;
    else if (nsrc <= '9')
      ndst = nsrc - '0';
    else if (nsrc < 'A')
      break;
    else if (nsrc <= 'Z')
      ndst = nsrc - 'A' + 10;
    else if (nsrc < 'a')
      break;
    else if (nsrc <= 'z')
      ndst = nsrc - 'a' + 10;
    else
      break;
    ndst <<= 4;
    srcoffset++;
    if (nsrc < '0')
      break;
    else if (nsrc <= '9')
      ndst |= nsrc - '0';
    else if (nsrc < 'A')
      break;
    else if (nsrc <= 'Z')
      ndst |= nsrc - 'A' + 10;
    else if (nsrc < 'a')
      break;
    else if (nsrc <= 'z')
      ndst |= nsrc - 'a' + 10;
    else
      break;
  }

  return dstoffset;

#undef nsrc
#undef ndst
#undef dst
}
// 十六进制解码
dlexport int hex_ndec(void *dst, size_t dstlen, cstr src, size_t srclen) {
  if (!dst || !src) return -1;
  if (srclen == 0 || dstlen == 0) return 0;
#define dst  ((byte *)dst)
#define nsrc (src[srcoffset])
#define ndst (dst[dstoffset])

  int dstoffset = 0;
  for (int srcoffset = 0; srcoffset < srclen && dstoffset < dstlen; srcoffset++, dstoffset++) {
    if (nsrc < '0')
      break;
    else if (nsrc <= '9')
      ndst = nsrc - '0';
    else if (nsrc < 'A')
      break;
    else if (nsrc <= 'Z')
      ndst = nsrc - 'A' + 10;
    else if (nsrc < 'a')
      break;
    else if (nsrc <= 'z')
      ndst = nsrc - 'a' + 10;
    else
      break;
    ndst <<= 4;
    if (++srcoffset >= srclen) break;
    if (nsrc < '0')
      break;
    else if (nsrc <= '9')
      ndst |= nsrc - '0';
    else if (nsrc < 'A')
      break;
    else if (nsrc <= 'Z')
      ndst |= nsrc - 'A' + 10;
    else if (nsrc < 'a')
      break;
    else if (nsrc <= 'z')
      ndst |= nsrc - 'a' + 10;
    else
      break;
  }

  return dstoffset;

#undef nsrc
#undef ndst
#undef dst
}
