#include "kernel/log.h"
#include <kernel.h>

void idle() {
  for (;;) {
    logd("A");
  }
}
void init() {
  //  logd("init function has been called successfully!");
  for (;;) {
    logd("B");
  }
}