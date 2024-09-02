#include <stdint.h>
#include <stdio.h>

void generate_crc16_table(uint16_t *crc16_table) {
  uint16_t polynomial = 0xA001;
  for (uint16_t i = 0; i < 256; i++) {
    uint16_t crc = i;
    for (uint16_t j = 8; j > 0; j--) {
      if (crc & 1) {
        crc = (crc >> 1) ^ polynomial;
      } else {
        crc >>= 1;
      }
    }
    crc16_table[i] = crc;
  }
}

void generate_crc32_table(uint32_t *crc32_table) {
  uint32_t polynomial = 0xEDB88320;
  for (uint32_t i = 0; i < 256; i++) {
    uint32_t crc = i;
    for (uint32_t j = 8; j > 0; j--) {
      if (crc & 1) {
        crc = (crc >> 1) ^ polynomial;
      } else {
        crc >>= 1;
      }
    }
    crc32_table[i] = crc;
  }
}

int main() {

  uint16_t crc16_table[256];
  generate_crc16_table(crc16_table);
  printf("crc16:\n  ");
  for (int i = 0; i < 256; i++) {
    printf("0x%04x, ", crc16_table[i]);
  }
  printf("\n\n");

  uint32_t crc32_table[256];
  generate_crc32_table(crc32_table);
  printf("crc32:\n  ");
  for (int i = 0; i < 256; i++) {
    printf("0x%08x, ", crc32_table[i]);
  }
  printf("\n\n");

  return 0;
}
