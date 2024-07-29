#include <loader.h>

#define STACK_SIZE (40 * 1024)
static byte stack_top[STACK_SIZE];

void DOSLDR_MAIN();

void loader_main() {
  asm_set_sp(stack_top + STACK_SIZE);
  DOSLDR_MAIN();
}
