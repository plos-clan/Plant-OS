// This code is released under the MIT License

#include <libc-base.h>
#include <sys.h>

int main(int argc, char **argv) {
  printf("argc: %d\n", argc);
  for (int i = 0; i < argc; i++) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }
  printf("%p\n", aligned_alloc(4096, 114));
  return 0;
}
