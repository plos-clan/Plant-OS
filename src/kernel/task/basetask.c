#include <kernel.h>

void idle() {
  for (;;) {}
}
void init() {
  logd("init function has been called successfully!");
  printf("Hello Plant-OS!\n");
  // logd("%p", malloc(200));
  for (int i = 0; i < 20; i++) {
    printi("I=%d", i);
  }
  for (;;) {}
}
