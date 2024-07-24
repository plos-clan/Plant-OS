#include <kernel.h>

void idle() {
  for (;;) {}
}
void init() {
  logd("init function has been called successfully!");
  printf("Hello Plant-OS!\n");
  // logd("%p", malloc(200));
  u32 *vram = (u32 *) set_mode(1024,768,32);
  memset(vram,0xff,1024*768*4);
  for(;;);
  for (int i = 0; i < 20; i++) {
    printi("I=%d", i);
  }
  for (;;) {}
}
