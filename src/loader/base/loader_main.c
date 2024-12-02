#include <loader.h>

void abort() {
  infinite_loop {
    asm_cli, asm_hlt;
  }
}
