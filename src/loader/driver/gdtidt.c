#include <loader.h>

// GDTIDT的初始化

void set_segmdesc(SegmentDescriptor *sd, u32 limit, int base, int ar) {
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

void set_gatedesc(GateDescriptor *gd, size_t offset, int selector, int ar) {
  gd->offset_low   = offset & 0xffff;
  gd->selector     = selector;
  gd->dw_count     = (ar >> 8) & 0xff;
  gd->access_right = ar & 0xff;
  gd->offset_high  = (offset >> 16) & 0xffff;
  return;
}

void default_inthandler() __attr(naked);

void default_inthandler() {
  asm volatile("iret\n\t");
}

void init_gdtidt() {
  var gdt = (SegmentDescriptor *)GDT_ADDR;
  var idt = (GateDescriptor *)IDT_ADDR;

  // 初始化 GDT
  for (int i = 0; i < GDT_LEN; i++) {
    set_segmdesc(gdt + i, 0, 0, 0);
  }
  set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
  set_segmdesc(gdt + 2, 0xffffffff, 0x00000000, AR_CODE32_ER);
  set_segmdesc(gdt + 4, 0xffffffff, 0x00000000, AR_CODE32_ER);
  load_gdt(gdt, GDT_LEN);

  // 初始化 IDT
  for (size_t i = 0; i < IDT_LEN; i++) {
    set_gatedesc(idt + i, (size_t)default_inthandler, 2 * 8, AR_INTGATE32);
  }
  load_idt(idt, IDT_LEN); // 加载IDT表
}
