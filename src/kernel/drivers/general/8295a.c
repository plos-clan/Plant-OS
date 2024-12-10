#include <kernel.h>

static u16 irq_mask = 0xfffb; // 1111 1111 1111 1011
                              // 1: 禁用，0: 启用

finline void set_irq_mask(u16 mask) {
  asm_out8(PIC0_IMR, mask & 0xff);
  asm_out8(PIC1_IMR, mask >> 8);
}

void init_pic() {
  set_irq_mask(0xffff);

  asm_out8(PIC0_ICW1, 0x11);
  asm_out8(PIC0_ICW2, 0x20);
  asm_out8(PIC0_ICW3, 1 << 2);
  asm_out8(PIC0_ICW4, 0x01);

  asm_out8(PIC1_ICW1, 0x11);
  asm_out8(PIC1_ICW2, 0x28);
  asm_out8(PIC1_ICW3, 2);
  asm_out8(PIC1_ICW4, 0x01);

  set_irq_mask(irq_mask);
}

void send_eoi(int irq) {
  assert(irq >= 0 && irq < 16 && irq != 2);
  if (irq >= 8) {
    asm_out8(PIC1_OCW2, 0x60 | (irq - 8));
    asm_out8(PIC0_OCW2, 0x60 | 2);
  } else {
    asm_out8(PIC0_OCW2, 0x60 | irq);
  }
}

void irq_enable(int irq) {
  assert(irq >= 0 && irq < 16 && irq != 2);
  irq_mask &= ~MASK16(irq);
  set_irq_mask(irq_mask);
}

void irq_disable(int irq) {
  assert(irq >= 0 && irq < 16 && irq != 2);
  irq_mask |= MASK16(irq);
  set_irq_mask(irq_mask);
}
