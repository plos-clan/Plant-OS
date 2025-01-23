#include <libc.h>
#include <sys.h>

static bool isprint(char c) {
  return c >= 0x20 && c <= 0x7e;
}

int main(int argc, const char **argv) {
  if (argc != 2) {
    printf("Usage: %s filename\n", argv[0]);
    return 1;
  }
  isize size = __syscall(SYSCALL_FILE_SIZE, argv[1]);
  if (size < 0) {
    printf("File not found: %s\n", argv[1]);
    return 1;
  }
  byte *buf = (byte *)xmalloc(size);
  if (__syscall(SYSCALL_LOAD_FILE, argv[1], buf, size) < 0) {
    printf("Failed to load file: %s\n", argv[1]);
    free(buf);
    return 1;
  }
  printf("\033[1;35mHEX DUMP\033[0m  ");
  for (usize j = 0; j < 16; j++) {
    printf("\033[1;37m%02x\033[0m ", j);
  }
  printf(" \033[1;36mCHAR\033[0m\n");
  for (usize i = 0; i < size; i += 16) {
    printf("\033[1;33m%08x\033[0m  ", i);
    for (usize j = 0; j < 16; j++) {
      if (i + j < size) {
        printf("%02x ", buf[i + j]);
      } else {
        printf("   ");
      }
    }
    printf(" ");
    for (usize j = 0; j < 16; j++) {
      if (i + j >= size) break;
      if (isprint(buf[i + j])) {
        printf("\033[1;32m%c\033[0m", buf[i + j]);
      } else {
        printf("\033[1;30m.\033[0m");
      }
    }
    printf("\n");
  }
  free(buf);
  return 0;
}