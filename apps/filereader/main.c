
#include <libc.h>
#include <sys.h>
int main(int argc, const char **argv) {
  if (argc != 2) {
    printf("Usage: %s filename\n", argv[0]);
    return 1;
  }
  size_t size = __syscall(SYSCALL_FILE_SIZE, argv[1]);
  byte  *buff = (byte *)xmalloc(size);
  __syscall(SYSCALL_LOAD_FILE, argv[1], buff, size);
  for (int i = 0; i < size; i++) {
    printf("%c", buff[i]);
  }
  printf("\n");
  return 0;
}