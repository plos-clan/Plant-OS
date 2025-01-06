#include <define.h>
#include <libc-base/asm.h>

#ifndef __x86_64__
#  error "这是 amd64 的代码，必须编译到 amd64 目标"
#endif

finline void _putb(int c) {
  waituntil(asm_in8(0x3f8 + 5) & 0x20);
  asm_out8(0x3f8, c);
}

finline void _puts(cstr s) {
  for (usize i = 0; s[i] != '\0'; i++) {
    _putb(s[i]);
  }
}

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
#define AR_TSS32     0x0089

typedef struct SegmentDescriptor {
  u16  limit_low;
  u16  base_low;
  byte base_mid;
  byte access_right;
  byte limit_high;
  byte base_high;
#ifdef __x86_64__
  u32 base_upper;
  u32 reserved;
#endif
} SegmentDescriptor;

typedef struct GateDescriptor {
  u16  offset_low;
  u16  selector;
  byte dw_count;
  byte access_right;
  u16  offset_high;
#ifdef __x86_64__
  u32 offset_upper;
  u32 reserved;
#endif
} GateDescriptor;

void set_segmdesc(SegmentDescriptor *sd, usize limit, usize base, u32 ar) {
  if (limit > 0xfffff) {
    ar    |= 0x8000; // G_bit = 1
    limit /= 0x1000;
  }
  sd->limit_low    = limit & 0xffff;
  sd->base_low     = base & 0xffff;
  sd->base_mid     = (base >> 16) & 0xff;
  sd->access_right = ar & 0xff;
  sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
  sd->base_high    = (base >> 24) & 0xff;
#ifdef __x86_64__
  sd->base_upper = (base >> 32) & 0xffffffff;
  sd->reserved   = 0;
#endif
}

void set_gatedesc(GateDescriptor *gd, usize offset, u16 selector, u32 ar) {
  gd->offset_low   = offset & 0xffff;
  gd->selector     = selector;
  gd->dw_count     = (ar >> 8) & 0xff;
  gd->access_right = ar & 0xff;
  gd->offset_high  = (offset >> 16) & 0xffff;
#ifdef __x86_64__
  gd->offset_upper = (offset >> 32) & 0xffffffff;
  gd->reserved     = 0;
#endif
}

isize efi_main(void *ImageHandle, void *SystemTable) {
  asm_cli;

  _puts("\n----- Plant-OS -----\n");

  // 初始化 GDT
  var gdt = (SegmentDescriptor *)GDT_ADDR;
  for (usize i = 0; i < GDT_LEN; i++) {
    set_segmdesc(gdt + i, 0, 0, 0);
  }
  set_segmdesc(gdt + 1, U32_MAX, 0, AR_CODE32_ER);
  set_segmdesc(gdt + 2, U32_MAX, 0, AR_DATA32_RW);
  set_segmdesc(gdt + 3, U32_MAX, 0, AR_CODE32_ER | 3 << 5);
  set_segmdesc(gdt + 4, U32_MAX, 0, AR_DATA32_RW | 3 << 5);
  load_gdt(gdt, GDT_LEN);

  // 初始化 IDT
  var idt = (GateDescriptor *)IDT_ADDR;
  for (size_t i = 0; i < IDT_LEN; i++) {
    int ar = i >= 0x30 ? AR_INTGATE32 | 3 << 5 : AR_INTGATE32;
    set_gatedesc(idt + i, null, 16, ar);
  }
  load_idt(idt, IDT_LEN); // 加载IDT表

  _puts("Hello world!\n");

  infinite_loop;
  return 0;
}
