#pragma once
#include <define.h>

#include <libc-base.h>

#define IDT_ADDR     0x0026f800 // IDT 地址
#define IDT_LEN      256
#define GDT_ADDR     0x00270000 // GDT 地址
#define GDT_LEN      8192
#define ADR_BOTPAK   0x100000
#define LIMIT_BOTPAK 0x0007ffff
#define AR_DATA32_RW 0x4092
#define AR_DATA16_RW 0x0092
#define AR_CODE32_ER 0x409a
#define AR_CODE16_ER 0x009a
#define AR_INTGATE32 0x008e

typedef struct SegmentDescriptor {
  i16  limit_low, base_low;
  char base_mid, access_right;
  char limit_high, base_high;
} SegmentDescriptor;

typedef struct GateDescriptor {
  i16  offset_low, selector;
  char dw_count, access_right;
  i16  offset_high;
} GateDescriptor;

void init_gdtidt(void);
void set_gatedesc(GateDescriptor *gd, size_t offset, int selector, int ar);