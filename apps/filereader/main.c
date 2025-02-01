#include <libc.h>
#include <sys.h>

static bool isprint(char c) {
  return c >= 0x20 && c <= 0x7e;
}

static void print_data(const byte *data, usize current, usize len) {
  for (usize i = 0; i < len; i += 16) {
    printf("\033[1;33m%08x\033[0m  ", i + current);
    for (usize j = 0; j < 16; j++) {
      if (i + j < len) {
        printf("%02x ", data[i + j]);
      } else {
        printf("   ");
      }
    }
    printf(" ");
    for (usize j = 0; j < 16; j++) {
      if (i + j >= len) break;
      if (isprint(data[i + j]))
        printf("\033[1;32m%c\033[0m", data[i + j]);
      else
        printf("\033[1;30m.\033[0m");
    }
    printf("\n");
  }
}
int main(int argc, const char **argv) {

  if (argc != 2) {
    printf("Usage: %s filename\n", argv[0]);
    return 1;
  }
  int fd = __syscall(SYSCALL_OPEN, argv[1], 0, 0);
  if (fd < 0) {
    printf("Failed to open file: %s\n", argv[1]);
    return 1;
  }
  // isize size = __syscall(SYSCALL_FILE_SIZE, argv[1]);
  // if (size < 0) {
  //   printf("File not found: %s\n", argv[1]);
  //   return 1;
  // }

  printf("\033[1;35mHEX DUMP\033[0m  ");
  for (usize j = 0; j < 16; j++) {
    printf("\033[1;37m%02x\033[0m ", j);
  }
  printf(" \033[1;36mCHAR\033[0m\n");

  byte *buf   = malloc(1024);
  usize total = 0;
  while (true) {
    val len = __syscall(SYSCALL_READ, fd, buf, 1024);
    if (len == 0) break;
    print_data(buf, total, len);
    total += len;
  }
  free(buf);
  __syscall(SYSCALL_CLOSE, fd);
  return 0;
}