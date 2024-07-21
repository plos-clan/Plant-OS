#pragma once
#include <define.h>
#include <type.h>

#include <asm/asm.h>

int  io_load_eflags();
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
int  load_cr0(void);
void store_cr0(int cr0);

#define ADR_IDT      0x0026f800
#define LIMIT_IDT    0x000007ff
#define ADR_GDT      0x00270000
#define LIMIT_GDT    0x0000ffff
#define ADR_BOTPAK   0x100000
#define LIMIT_BOTPAK 0x0007ffff
#define AR_DATA32_RW 0x4092
#define AR_DATA16_RW 0x0092
#define AR_CODE32_ER 0x409a
#define AR_CODE16_ER 0x009a
#define AR_INTGATE32 0x008e

struct SEGMENT_DESCRIPTOR {
  i16  limit_low, base_low;
  char base_mid, access_right;
  char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
  i16  offset_low, selector;
  char dw_count, access_right;
  i16  offset_high;
};

void init_gdtidt(void);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);