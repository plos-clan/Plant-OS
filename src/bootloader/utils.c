#include "utils.h"
#include "uefi/uefi.h"

static int isprint(char c) {
  return c >= 0x20 && c <= 0x7e;
}

void dump_memory(const void *_buf, uintn_t size) {
  const uint8_t *buf = _buf;
  printf("\033[1;35mHEX DUMP\033[0m  ");
  for (size_t j = 0; j < 16; j++) {
    printf("\033[1;37m%02x\033[0m ", j);
  }
  printf(" \033[1;36mCHAR\033[0m\n");
  for (size_t i = 0; i < size; i += 16) {
    printf("\033[1;33m%08x\033[0m  ", i);
    for (size_t j = 0; j < 16; j++) {
      if (i + j < size) {
        printf("%02x ", buf[i + j]);
      } else {
        printf("   ");
      }
    }
    printf(" ");
    for (size_t j = 0; j < 16; j++) {
      if (i + j >= size) break;
      if (isprint(buf[i + j])) {
        printf("\033[1;32m%c\033[0m", buf[i + j]);
      } else {
        printf("\033[1;30m.\033[0m");
      }
    }
    printf("\n");
  }
}
