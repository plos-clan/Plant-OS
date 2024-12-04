#include <cstdio>
#include <cstdlib>

#define NO_STD 0
#include <plmp/mpi32.hpp>

finline cstr tostring(u32 *_rest a, size_t len) {
  static char buf[1024];
  // memset(buf, 1, 1024);
  auto        ptr = cpp::mpi_tostr(buf, 1024, a, len, 10, true);
  // printf("%p %p\n", buf + 1024, ptr);
  // for (size_t i = 0; i < 1024; i++) {
  //   printf("%d ", buf[i]);
  // }
  return ptr;
}

int main() {
  // cpp::mpi<1024> a   = 1;
  // a                <<= 9;
  // // a                 -= 1;
  // printf("%s\n", tostring(a.data, a.len));

  MPI(1024, a);
  cpp::mpi_set1(a, LEN_MPI(1024));
  cpp::mpi_lshift2(a, 127, LEN_MPI(1024));
  cpp::mpi_sub2i(a, 1, LEN_MPI(1024));
  printf("%s\n", tostring(a, LEN_MPI(1024)));
}
