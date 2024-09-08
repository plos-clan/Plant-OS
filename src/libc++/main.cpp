#include <libc-base.hpp>

#ifdef __clang__

int main() __attr(weak);
int main(int argc, char **argv) __attr(weak);
int main(int argc, char **argv, char **envp) __attr(weak);

int main() {
  return 0;
}

int main(int argc, char **argv) {
  return 0;
}

int main(int argc, char **argv, char **envp) {
  return 0;
}

#endif
