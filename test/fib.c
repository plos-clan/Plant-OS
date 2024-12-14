#include <stdio.h>
#include <stdlib.h>

#define NO_STD 0
#include <plmp/mpi32.h>

const char _plos_lut_alnum_lower[62] =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char _plos_lut_alnum_upper[62] =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

finline cstr tostring(u32 *_rest a, size_t len) {
  static char buf[1024];
  return mpi_tostr(buf, 1024, a, len, 10, true);
}

#define NBITS 1024
#define LEN   LEN_MPI(NBITS)

void fib(u32 *result, i32 x) {
  x -= 3;
  if (x < 0) {
    mpi_set1(result, LEN);
    return;
  }
  u32 *a  = malloc(NBITS / 8);
  u32 *b  = malloc(NBITS / 8);
  u32 *c  = malloc(NBITS / 8);
  u32 *a2 = malloc(NBITS / 8);
  u32 *b2 = malloc(NBITS / 8);
  u32 *c2 = malloc(NBITS / 8);
  u32 *i  = malloc(NBITS / 8);
  u32 *j  = malloc(NBITS / 8);
  u32 *t1 = malloc(NBITS / 8);
  u32 *t2 = malloc(NBITS / 8);
  u32 *t3 = malloc(NBITS / 8);
  u32 *t4 = malloc(NBITS / 8);

  mpi_set0(a, LEN);
  mpi_set1(b, LEN);
  mpi_set1(c, LEN);
  mpi_set1(i, LEN);
  mpi_set1(j, LEN);

  while (x > 0) {
    if (x & 1) {
      mpi_mul3(t1, a, i, LEN);
      mpi_mul3(t2, b, j, LEN);
      mpi_mul3(t3, b, i, LEN);
      mpi_mul3(t4, c, j, LEN);
      mpi_add3(i, t1, t2, LEN);
      mpi_add3(j, t3, t4, LEN);
    }
    mpi_mul3(a2, a, a, LEN);
    mpi_mul3(b2, b, b, LEN);
    mpi_mul3(c2, c, c, LEN);
    mpi_add3(t1, a, c, LEN);

    mpi_add3(a, a2, b2, LEN);

    mpi_set(t2, b, LEN);
    mpi_mul3(b, t1, t2, LEN);

    mpi_add3(c, b2, c2, LEN);

    x >>= 1;
  }

  mpi_add3(result, i, j, LEN);

  free(a);
  free(b);
  free(c);
  free(a2);
  free(b2);
  free(c2);
  free(i);
  free(j);
  free(t1);
  free(t2);
  free(t3);
  free(t4);
}

int main() {
  u32 *result = malloc(NBITS / 8);
  fib(result, 400);
  printf("%s\n", tostring(result, LEN));
  free(result);
}
