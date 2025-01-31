// This code is released under the MIT License

#include <libc.h>

typedef int (*main_t)(int argc, char **argv, char **envp);

int main(int argc, char **argv, char **envp);

dlimport bool __libc_init_mman();

dlexport void __libc_start_main(int argc, char **argv, char **envp) {
  __libc_init_mman();

  // exit(1);

  val ret = main(argc, argv, envp);

  exit(ret);
}
