#include <kernel.h>

static void play_sound(u32 nFrequence) {
  u32 Div;
  u8 tmp;
  
  Div = 1193180 / nFrequence;
  asm_out8(0x43, 0xb6);
  asm_out8(0x42, (uint8_t) (Div) );
  asm_out8(0x42, (uint8_t) (Div >> 8));

  tmp = asm_in8(0x61);
  if (tmp != (tmp | 3)) {
    asm_out8(0x61, tmp | 3);
  }
}

static void nosound() {
  u8 tmp = asm_in8(0x61) & 0xFC;
  asm_out8(0x61, tmp);
}

void beepX(u32 n,u32 d){
  play_sound(n);
  //TODO sleep(d); sleep函数的实现找不到
  nosound();
}

void beep() {
  play_sound(1175);
  //TODO sleep(10); sleep函数的实现找不到
  nosound();
}