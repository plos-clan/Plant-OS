// This code is released under the MIT License

#include <libc-base.h>
#include <magic.h>
#include <sys.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: %s <file>\n", argv[0]);
    return 1;
  }

  // 先等 syscall 完善

  return 0;
}
