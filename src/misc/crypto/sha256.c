#include <misc.h>

#define R(v, n)      ((v) >> (n))                              // 右移
#define S(v, n)      (((v) >> (n)) | ((v) << (32 - (n))))      // 循环右移
#define CH(e, f, g)  (((e) & (f)) ^ ((~(e)) & (g)))            //
#define MAJ(a, b, c) (((a) & (c)) ^ ((a) & (b)) ^ ((b) & (c))) //
#define SIGMA0(x)    (S(x, 2) ^ S(x, 13) ^ S(x, 22))           //
#define SIGMA1(x)    (S(x, 6) ^ S(x, 11) ^ S(x, 25))           //
#define GAMMA0(x)    (S(x, 7) ^ S(x, 18) ^ R(x, 3))            //
#define GAMMA1(x)    (S(x, 17) ^ S(x, 19) ^ R(x, 10))          //

static const u32 SHA256_H[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, //
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19, //
};

static const u32 SHA256_K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, //
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, //
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, //
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, //
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, //
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, //
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, //
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, //
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, //
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, //
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, //
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, //
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, //
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, //
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, //
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2, //
};

static void SHA256_iterate(SHA256_t *_rest s) noexcept {
  u32 temp1, temp2; // 临时变量
  u32 word[64];     // （SHA256）字

  // 生成每一个（SHA256）字
  for (int i = 0; i < 16; i++) {
    word[i] = big_endian(((u32 *)s->b)[i]);
  }
  for (int i = 16; i < 64; i++) {
    word[i] = word[i - 16] + GAMMA0(word[i - 15]) + word[i - 7] + GAMMA1(word[i - 2]);
  }

  u32 h[8]; // 临时变量
  *(v8u32 *)h = *(v8u32 *)s->h;

  for (int i = 0; i < 64; i++) { // 计算每一个（SHA256）字
    temp1 = h[7] + SIGMA1(h[4]) + CH(h[4], h[5], h[6]) + SHA256_K[i] + word[i];
    temp2 = SIGMA0(h[0]) + MAJ(h[0], h[1], h[2]);
    h[7]  = h[6];
    h[6]  = h[5];
    h[5]  = h[4];
    h[4]  = h[3] + temp1;
    h[3]  = h[2];
    h[2]  = h[1];
    h[1]  = h[0];
    h[0]  = temp1 + temp2;
  }

  *(v8u32 *)s->h += *(v8u32 *)h; // 保存迭代结果
}

void SHA256_init(SHA256_t *_rest s) noexcept {
  if (!s) return;
  *s = (SHA256_t){
      {},
      0,
      0,
      {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab,
       0x5be0cd19}
  };
}

void SHA256_input(SHA256_t *_rest s, const void *_rest data, size_t len) noexcept {
  if (!s || !data || !len) return;
  for (size_t i = 0; i < len; i++) {
    s->b[s->p++] = ((const byte *)data)[i];
    if (s->p == 64) {
      SHA256_iterate(s);
      s->p = 0;
    }
  }
  s->s += len;
}

const void *SHA256_end(SHA256_t *_rest s) noexcept {
  if (!s) return NULL;
  s->b[s->p++] = 0x80;
  for (; s->p != 56; s->p++) {
    if (s->p == 64) {
      SHA256_iterate(s);
      s->p = 0;
    }
    s->b[s->p] = 0;
  }
  *(u64 *)(s->b + 56) = big_endian(s->s * 8);
  SHA256_iterate(s);
  return s->h;
}

const void *SHA256_val(const SHA256_t *_rest s) noexcept {
  if (!s) return NULL;
  return s->h;
}

void SHA256_print(const SHA256_t *_rest s) noexcept {
  if (!s) return;
  for (int i = 0; i < 8; i++) {
    printf("%08x", s->h[i]);
  }
  printf("\n");
}

const void *SHA256(SHA256_t *_rest s, const void *_rest data, size_t len) noexcept {
  if (!s) return NULL;
  SHA256_init(s);
  SHA256_input(s, data, len);
  return SHA256_end(s);
}
