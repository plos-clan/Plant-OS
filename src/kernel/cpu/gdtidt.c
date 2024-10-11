#include <kernel.h>

void empty_inthandler();
void ide_irq();

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
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, size_t offset, int selector, int ar) {
  gd->offset_low   = offset & 0xffff;
  gd->selector     = selector;
  gd->dw_count     = (ar >> 8) & 0xff;
  gd->access_right = ar & 0xff;
  gd->offset_high  = (offset >> 16) & 0xffff;
}

static const void *handlers[256] = {
    [0x00] = asm_error0,       [0x01] = asm_error1,  [0x03] = asm_error3,  [0x04] = asm_error4,
    [0x05] = asm_error5,       [0x06] = asm_error6,  [0x07] = asm_error7,  [0x08] = asm_error8,
    [0x09] = asm_error9,       [0x0a] = asm_error10, [0x0b] = asm_error11, [0x0c] = asm_error12,
    [0x0d] = asm_error13,      [0x0e] = asm_error14, [0x10] = asm_error16, [0x11] = asm_error17,
    [0x12]      = asm_error18,
    [0x20]      = asm_inthandler20, // 计时器中断
    [0x21]      = asm_inthandler21, // 键盘中断
    [0x36]      = asm_inthandler36, // 系统API
    [0x72]      = asm_inthandler72, // 系统API
    [0x2c]      = asm_inthandler2c, // 鼠标中断
    [0x20 + 14] = asm_ide_irq,      // IDE中断
    [0x20 + 15] = asm_ide_irq,      // IDE中断
};

void init_gdtidt() {
  var gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
  var idt = (struct GATE_DESCRIPTOR *)ADR_IDT;

  // 初始化 GDT
  for (int i = 0; i < GDT_LEN; i++) {
    set_segmdesc(gdt + i, 0, 0, 0);
  }

  set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
  set_segmdesc(gdt + 2, 0xffffffff, 0x00000000, AR_CODE32_ER);
  set_segmdesc(gdt + 3, 0xffffffff, 0x00000000, AR_DATA32_RW | 3 << 5);
  set_segmdesc(gdt + 4, 0xffffffff, 0x00000000, AR_CODE32_ER | 3 << 5);
  set_segmdesc(gdt + 5, 0xffffffff, 0x70000000, AR_DATA32_RW | 3 << 5);
  set_segmdesc(gdt + 1000, 0xffffffff, 0, AR_CODE32_ER); // CODE32
  set_segmdesc(gdt + 1001, 0xfffff, 0, AR_CODE16_ER);    // CODE16
  set_segmdesc(gdt + 1002, 0xfffff, 0, AR_DATA16_RW);    // DATA16
  load_gdt(gdt, GDT_LEN);                                // 加载GDT表

  // 初始化 IDT
  for (int i = 0; i < IDT_LEN; i++) {
    set_gatedesc(idt + i, 0, 0, 0);
  }
  load_idt(idt, IDT_LEN); // 加载IDT表

  for (size_t i = 0; i < lengthof(handlers); i++) {
    set_gatedesc(idt + i, (size_t)handlers[i] ?: (size_t)empty_inthandler, 2 * 8, AR_INTGATE32);
  }

  set_gatedesc(idt + 0x30, (int)asm_net_api, 2 * 8, AR_INTGATE32 | 3 << 5);
}

// 注册中断处理函数
void register_intr_handler(int num, int addr) {
  struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *)ADR_IDT;
  set_gatedesc(idt + num, addr, 2 * 8, AR_INTGATE32);
}
