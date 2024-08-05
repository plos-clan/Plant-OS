#include <libc.h>

dlimport bool __libc_init_mman();

dlimport int main(int argc, char **argv, char **envp);

void __libc_start_main() {
  int    argc;
  char **argv;
  char **envp;
  asm("mov %%edi, %0\n\t" : "=r"(argc));
  asm("mov %%esi, %0\n\t" : "=r"(argv));
  asm("mov %%edx, %0\n\t" : "=r"(envp));

  __libc_init_mman();

  int ret = main(argc, argv, envp);

  exit(ret);
}
