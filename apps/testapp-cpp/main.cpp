// This code is released under the MIT License

#include <libc-base.hpp>
#include <pl2d.hpp>
#include <sys.h>

int main(int argc, char **argv) {
  printf("argc: %d\n", argc);
  for (int i = 0; i < argc; i++) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }
  return 0;
}
