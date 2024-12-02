#include <misc.h>

#undef I

#define L(v, n) (((v) << (n)) | ((v) >> (32 - (n)))) // 循环左移

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

//~ 循环左移的位数
static const byte MD5_S_v[4][4] = {
    {7, 12, 17, 22},
    {5, 9,  14, 20},
    {4, 11, 16, 23},
    {6, 10, 15, 21}
};

//~ 每轮循环中子组处理顺序
static const byte MD5_oi[4][16] = {
    {0, 1, 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15},
    {1, 6, 11, 0,  5,  10, 15, 4,  9,  14, 3,  8,  13, 2,  7,  12},
    {5, 8, 11, 14, 1,  4,  7,  10, 13, 0,  3,  6,  9,  12, 15, 2 },
    {0, 7, 14, 5,  12, 3,  10, 1,  8,  15, 6,  13, 4,  11, 2,  9 }
};

static const u32 MD5_ac[4][16] = {
    {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
     0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e,
     0x49b40821},
    {0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
     0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9,
     0x8d2a4c8a},
    {0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
     0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8,
     0xc4ac5665},
    {0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
     0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb,
     0xeb86d391}
};

static void MD5_iterate(MD5_t *_rest m) noexcept {
  u32 temp; // 临时变量

  u32 h[4]; // 临时变量
  *(v4u32 *)h = *(v4u32 *)m->h;

  // 第1轮变换
  for (int i = 0; i < 16; i++) {
    temp  = h[0] + F(h[1], h[2], h[3]) + ((u32 *)m->b)[i] + MD5_ac[0][i];
    temp  = L(temp, MD5_S_v[0][i % 4]);
    h[0]  = h[3];
    h[3]  = h[2];
    h[2]  = h[1];
    h[1] += temp;
  }
  // 第2轮变换
  for (int i = 0; i < 16; i++) {
    temp  = h[0] + G(h[1], h[2], h[3]) + ((u32 *)m->b)[(5 * i + 1) % 16] + MD5_ac[1][i];
    temp  = L(temp, MD5_S_v[1][i % 4]);
    h[0]  = h[3];
    h[3]  = h[2];
    h[2]  = h[1];
    h[1] += temp;
  }
  // 第3轮变换
  for (int i = 0; i < 16; i++) {
    temp  = h[0] + H(h[1], h[2], h[3]) + ((u32 *)m->b)[(3 * i + 5) % 16] + MD5_ac[2][i];
    temp  = L(temp, MD5_S_v[2][i % 4]);
    h[0]  = h[3];
    h[3]  = h[2];
    h[2]  = h[1];
    h[1] += temp;
  }
  // 第4轮变换
  for (int i = 0; i < 16; i++) {
    temp  = h[0] + I(h[1], h[2], h[3]) + ((u32 *)m->b)[(7 * i) % 16] + MD5_ac[3][i];
    temp  = L(temp, MD5_S_v[3][i % 4]);
    h[0]  = h[3];
    h[3]  = h[2];
    h[2]  = h[1];
    h[1] += temp;
  }

  // 保存迭代结果
  *(v4u32 *)m->h += *(v4u32 *)h;
}

void MD5_init(MD5_t *_rest m) noexcept {
  if (!m) return;
  *m = (MD5_t){
      {},
      0, 0, {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476}
  };
}

void MD5_input(MD5_t *_rest m, const void *_rest data, size_t len) noexcept {
  if (!m || !data || !len) return;
  for (size_t i = 0; i < len; i++) {
    m->b[m->p++] = ((const byte *)data)[i];
    if (m->p == 64) {
      MD5_iterate(m);
      m->p = 0;
    }
  }
  m->s += len;
}

const void *MD5_end(MD5_t *_rest m) noexcept {
  if (!m) return NULL;
  m->b[m->p++] = 0x80;
  for (; m->p != 56; m->p++) {
    if (m->p == 64) {
      MD5_iterate(m);
      m->p = 0;
    }
    m->b[m->p] = 0;
  }
  *(u64 *)(m->b + 56) = m->s * 8;
  MD5_iterate(m);
  return m->h;
}

const void *MD5_val(const MD5_t *_rest m) noexcept {
  if (!m) return NULL;
  return m->h;
}

void MD5_print(const MD5_t *_rest m) noexcept {
  if (!m) return;
  for (int i = 0; i < 16; i++) {
    printf("%02x", ((byte *)m->h)[i]);
  }
  printf("\n");
}

const void *MD5(MD5_t *_rest m, const void *_rest data, size_t len) noexcept {
  if (!m) return NULL;
  MD5_init(m);
  MD5_input(m, data, len);
  return MD5_end(m);
}
