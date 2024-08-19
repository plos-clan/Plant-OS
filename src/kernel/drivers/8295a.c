#include <kernel.h>

u8 value_pic0 = 0xfb;
u8 value_pic1 = 0xff;

void init_pic() {
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

  asm_out8(PIC0_IMR, value_pic0);
  asm_out8(PIC1_IMR, value_pic1); /* 禁止所有中断 */
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
  u16 port  = irq < 8 ? PIC0_IMR : PIC1_IMR;
  u8 *value = irq < 8 ? &value_pic0 : &value_pic1;
  if (irq >= 8) irq -= 8;
  *value &= ~(1 << irq);
  asm_out8(port, *value);
}

void irq_mask_set(u8 irq) {
  u16 port  = irq < 8 ? PIC0_IMR : PIC1_IMR;
  u8 *value = irq < 8 ? &value_pic0 : &value_pic1;
  if (irq >= 8) irq -= 8;
  *value |= (1 << irq);
  asm_out8(port, *value);
}
