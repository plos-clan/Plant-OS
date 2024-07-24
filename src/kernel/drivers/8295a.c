#include <kernel.h>

void init_pic(void) {
  asm_out8(PIC0_IMR, 0xff); /* 初始化，所有中断均被屏蔽 */
  asm_out8(PIC1_IMR, 0xff);
  asm_out8(PIC0_ICW1, 0x11);
  asm_out8(PIC0_ICW2, 0x20);
  asm_out8(PIC0_ICW3, 1 << 2);
  asm_out8(PIC0_ICW4, 0x01);

  asm_out8(PIC1_ICW1, 0x11);
  asm_out8(PIC1_ICW2, 0x28);
  asm_out8(PIC1_ICW3, 2);
  asm_out8(PIC1_ICW4, 0x01);

  asm_out8(PIC0_IMR, 0xfb);
  asm_out8(PIC1_IMR, 0xff); /* 禁止所有中断 */
  return;
}

void send_eoi(int irq) {
  if (irq >= 8) {
    asm_out8(PIC1_OCW2, 0x60 | (irq - 8));
    asm_out8(PIC0_OCW2, 0x60 | 2);
  } else {
    asm_out8(PIC0_OCW2, 0x60 | irq);
  }
}

void irq_mask_clear(u8 irq) {
  u16 port;
  u8  value;
  if (irq < 8) {
    port = PIC0_IMR;
  } else {
    port  = PIC1_IMR;
    irq  -= 8;
  }
  value = asm_in8(port) & ~(1 << irq);
  asm_out8(port, value);
}

void irq_mask_set(u8 irq) {
  u16 port;
  u8  value;
  if (irq < 8) {
    port = PIC0_IMR;
  } else {
    port  = PIC1_IMR;
    irq  -= 8;
  }
  value = asm_in8(port) | (1 << irq);
  asm_out8(port, value);
}
