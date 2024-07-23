#include <kernel.h>
void null_inthandler();
void ide_irq();
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar) {
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
  set_segmdesc(gdt + 3, 0xffffffff, 0x00000000, AR_DATA32_RW | 3 << 5);
  set_segmdesc(gdt + 4, 0xffffffff, 0x00000000, AR_CODE32_ER | 3 << 5);
  set_segmdesc(gdt + 5, 0xffffffff, 0x70000000, AR_DATA32_RW | 3 << 5);
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
  // 接下来这里是设置中断处理函数的地址
  set_gatedesc(idt + 0x00, (int)asm_error0, 2 * 8, AR_INTGATE32); // 0x00号异常
  set_gatedesc(idt + 0x01, (int)asm_error1, 2 * 8, AR_INTGATE32); // 0x01号异常
  set_gatedesc(idt + 0x03, (int)asm_error3, 2 * 8, AR_INTGATE32); // 0x03号异常
  set_gatedesc(idt + 0x04, (int)asm_error4, 2 * 8, AR_INTGATE32); // 0x04号异常
  set_gatedesc(idt + 0x05, (int)asm_error5, 2 * 8, AR_INTGATE32); // 0x05号异常
  set_gatedesc(idt + 0x06, (int)asm_error6, 2 * 8, AR_INTGATE32); // 0x06号异常
  set_gatedesc(idt + 0x07, (int)asm_error7, 2 * 8, AR_INTGATE32); // 0x07号异常
  set_gatedesc(idt + 0x08, (int)asm_error8, 2 * 8, AR_INTGATE32); // 0x08号异常
  set_gatedesc(idt + 0x09, (int)asm_error9, 2 * 8, AR_INTGATE32); // 0x09号异常
  set_gatedesc(idt + 0x0a, (int)asm_error10, 2 * 8,
               AR_INTGATE32); // 0x0a号异常
  set_gatedesc(idt + 0x0b, (int)asm_error11, 2 * 8,
               AR_INTGATE32); // 0x0b号异常
  set_gatedesc(idt + 0x0c, (int)asm_error12, 2 * 8,
               AR_INTGATE32); // 0x0c号异常
  set_gatedesc(idt + 0x0d, (int)asm_error13, 2 * 8,
               AR_INTGATE32); // 0x0d号异常
  set_gatedesc(idt + 0x0e, (int)asm_error14, 2 * 8,
               AR_INTGATE32); // 0x0e号异常
  set_gatedesc(idt + 0x10, (int)asm_error16, 2 * 8,
               AR_INTGATE32); // 0x10号异常
  set_gatedesc(idt + 0x11, (int)asm_error17, 2 * 8,
               AR_INTGATE32); // 0x11号异常
  set_gatedesc(idt + 0x12, (int)asm_error18, 2 * 8,
               AR_INTGATE32); // 0x12号异常
  set_gatedesc(idt + 0x20, (int)asm_inthandler20, 2 * 8,
               AR_INTGATE32); // 计时器中断
  set_gatedesc(idt + 0x21, (int)asm_inthandler21, 2 * 8,
               AR_INTGATE32); // 键盘中断
  set_gatedesc(idt + 0x36, (int)asm_inthandler36, 2 * 8,
               AR_INTGATE32 | 3 << 5); // 系统API
  set_gatedesc(idt + 0x72, (int)asm_inthandler72, 2 * 8,
               AR_INTGATE32 | 3 << 5); // 系统API
  set_gatedesc(idt + 0x2c, (int)asm_inthandler2c, 2 * 8,
               AR_INTGATE32); // 鼠标中断
  set_gatedesc(idt + 0x20 + 15, (int)asm_ide_irq, 2 * 8,
               AR_INTGATE32); // IDE中断
  set_gatedesc(idt + 0x20 + 14, (int)asm_ide_irq, 2 * 8,
               AR_INTGATE32); // IDE中断
  set_gatedesc(idt + 0x30, (int)asm_net_api, 2 * 8,
               AR_INTGATE32 | 3 << 5); // NET API
  return;
}
// 注册中断处理函数
void register_intr_handler(int num, int addr) {
  struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *)ADR_IDT;
  set_gatedesc(idt + num, addr, 2 * 8, AR_INTGATE32);
}