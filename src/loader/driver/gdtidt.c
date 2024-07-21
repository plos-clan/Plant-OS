
// GDTIDT的初始化
//  Copyright (C) 2021-2022 zhouzhihao & min0911_
//  ------------------------------------------------
#include <dosldr.h>
void null_inthandler();
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, u32 limit, int base, int ar) {
  if (limit > 0xfffff) {
    ar    |= 0x8000; /* G_bit = 1 */
    limit /= 0x1000;
  }
  sd->limit_low    = limit & 0xffff;
  sd->base_low     = base & 0xffff;
  sd->base_mid     = (base >> 16) & 0xff;
  sd->access_right = ar & 0xff;
  sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
  sd->base_high    = (base >> 24) & 0xff;
  return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar) {
  gd->offset_low   = offset & 0xffff;
  gd->selector     = selector;
  gd->dw_count     = (ar >> 8) & 0xff;
  gd->access_right = ar & 0xff;
  gd->offset_high  = (offset >> 16) & 0xffff;
  return;
}

void init_gdtidt(void) {
  struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
  struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR *)ADR_IDT;
  int                        i;

  /* GDT初始化 */
  for (i = 0; i <= LIMIT_GDT / 8; i++) {
    set_segmdesc(gdt + i, 0, 0, 0);
  }
  set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
  set_segmdesc(gdt + 2, 0xffffffff, 0x00000000, AR_CODE32_ER);
  set_segmdesc(gdt + 4, 0xffffffff, 0x00000000, AR_CODE32_ER);
  load_gdtr(LIMIT_GDT, ADR_GDT); // 加载GDT表

  /* IDT */
  for (i = 0; i <= LIMIT_IDT / 8; i++) {
    set_gatedesc(idt + i, 0, 0, 0);
  }
  load_idtr(LIMIT_IDT, ADR_IDT); // 加载IDT表
  /* IDT初始化*/
  for (int i = 0; i < 0xff; i++) {
    // 为了防止报#GP异常，将所有中断处理函数的地址设置为0
    set_gatedesc(idt + i, (int)null_inthandler, 2 * 8, AR_INTGATE32);
  }
  return;
}
// 注册中断处理函数
void register_intr_handler(int num, int addr) {
  struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *)ADR_IDT;
  set_gatedesc(idt + num, addr, 2 * 8, AR_INTGATE32);
}
