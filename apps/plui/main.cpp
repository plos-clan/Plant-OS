#include <pl2d.hpp>
#include <sys.h>

int main() {
  void *vram;
  int   width, height;
  __syscall(SYSCALL_VBE_GETMODE, &vram, &width, &height);
  printf("vram=%p, width=%d, height=%d\n", vram, width, height);
  lgmemset32((u32 *)vram, 0x00ffffff, width * height);
  return 0;
}
