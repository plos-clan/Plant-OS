#include <kernel.h>

void idle() {
  for (;;) {}
}
void init() {
  logd("init function has been called successfully!");
  logd("%p", malloc(200));
  for (;;) {}
}
