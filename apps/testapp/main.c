// This code is released under the MIT License

#include <libc-base.h>
#include <sys.h>

int main(int argc, char **argv) {
  printf("cmdline: %s\n", task_command_line);
  printf("argc: %d\n", argc);
  for (int i = 0; i < argc; i++) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }
  printf("%p\n", aligned_alloc(4096, 114));
  printf("tid: %d\n", task_user_part->tid);

  if (__syscall(SYSCALL_FORK)) {
    printf("parent\n");
  } else {
    printf("child\n");
  }

  printf("%f\n", sin(1));
  printf("%f\n", log(1));

  return 0;
}
