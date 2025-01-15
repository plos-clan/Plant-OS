// This code is released under the MIT License

#include <libc.h>

dlimport bool __libc_init_mman();

dlexport void __libc_start_main(int argc, char **argv, char **envp,
                                int (*main)(int argc, char **argv, char **envp)) {
  __libc_init_mman();

  val ret = main(argc, argv, envp);

  exit(ret);
}
