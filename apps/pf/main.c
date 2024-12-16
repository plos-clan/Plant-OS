// This code is released under the MIT License

#include <libc-base.h>
#include <sys.h>

int main(int argc, char **argv) {
  mem_set(0x114514, 1);
  return 0;
}
