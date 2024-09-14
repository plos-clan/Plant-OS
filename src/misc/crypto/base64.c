#include <misc.h>

static const char b64_lut[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const byte b64_rlut[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 1
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, // 2
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -2, -1, -1, // 3
    -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, // 4
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, // 5
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, // 6
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, // 7
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 8
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 9
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // a
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // b
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // c
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // d
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // e
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // f
};

// base64编码
// 参数错误，返回-1
// 目标缓冲区空间不足，返回 1
// 成功，返回 0
dlexport int base64_enc(char *_rest dst, size_t dstlen, const void *_rest src, size_t srclen) {
  if (!dst || !src) return -1;
  if (!srclen) return 0;

  size_t sizereq = (srclen + 2) / 3 * 4;
  if (dstlen < sizereq) return 1;

  const byte *s = src;
  char       *d = dst;

  size_t i = 0;
  for (; i < srclen - 2; i += 3) {
    *(d++) = b64_lut[s[i] >> 2];
    *(d++) = b64_lut[((s[i] << 4) | (s[i + 1] >> 4)) & 63];
    *(d++) = b64_lut[((s[i + 1] << 2) | (s[i + 2] >> 6)) & 63];
    *(d++) = b64_lut[s[i + 2] & 63];
  }
  if (i < srclen) {
    *(d++) = b64_lut[s[i] >> 2];
    if (i == srclen - 1) {
      *(d++) = b64_lut[(s[i] << 4) & 63];
      *(d++) = '=';
      *(d++) = '=';
    } else {
      *(d++) = b64_lut[((s[i] << 4) | (s[i + 1] >> 4)) & 63];
      *(d++) = b64_lut[(s[i + 1] << 2) & 63];
      *(d++) = '=';
    }
  }

  if (dstlen > sizereq) *d = '\0';

  return 0;
}

// base64解码
// 参数错误，返回-1
// 目标缓冲区空间不足，返回 1
// 成功，返回 0
dlexport int base64_dec(void *_rest dst, size_t dstlen, cstr _rest src, size_t srclen) {
  if (!dst || !src) return -1;
  if (!srclen) return 0;

  if (srclen % 4 != 0) return -1;

  size_t sizereq = srclen / 4 * 3;
  if (dstlen < sizereq) return 1;

  cstr  s = src;
  byte *d = dst;

  byte b0, b1, b2, b3;

  for (size_t i = 0; i < srclen; i += 4) {
    if (b0 = b64_rlut[*(s++)], b0 > 0xf0) return -1;
    if (b1 = b64_rlut[*(s++)], b1 > 0xf0) return -1;
    if (b2 = b64_rlut[*(s++)], b2 == 0xff) return -1;
    if (b3 = b64_rlut[*(s++)], b3 == 0xff) return -1;

    *(d++) = (b0 << 2) | (b1 >> 4);
    *(d++) = (b1 << 4) | (b2 >> 2);
    *(d++) = (b2 << 6) | (b3);
  }
  if (b3 == 0xfe) {
    *(--d) = 0;
    if (b2 == 0xfe)
      *(--d) = 0;
    else
      return -1;
  }

  return 0;
}
