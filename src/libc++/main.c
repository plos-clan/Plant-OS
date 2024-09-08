#include <libc-base.h>

#ifdef __clang__

int _Z4mainv();
int _Z4mainiPPc(int argc, char **argv);
int _Z4mainiPPcS0_(int argc, char **argv, char **envp);

int main(int argc, char **argv, char **envp) {
  int ret  = 0;
  ret     |= _Z4mainv();
  ret     |= _Z4mainiPPc(argc, argv);
  ret     |= _Z4mainiPPcS0_(argc, argv, envp);
  return ret;
}

#endif
