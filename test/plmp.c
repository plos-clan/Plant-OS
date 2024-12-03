#include <stdio.h>
#include <stdlib.h>

#define NO_STD 0
#include <plmp.h>

const char _plos_lut_alnum_lower[62] =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char _plos_lut_alnum_upper[62] =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

finline cstr tostring(u32 *_rest a, size_t len) {
  static char buf[1024];
  return mpi_tostr(buf, 1024, a, len, 10, true);
}

int main() {
  MPI(1024, a);
  mpi_set1(a, LEN_MPI(1024));
  mpi_lshift2(a, 127, LEN_MPI(1024));
  mpi_sub2i(a, 1, LEN_MPI(1024));
  printf("%s\n", tostring(a, LEN_MPI(1024)));
}
