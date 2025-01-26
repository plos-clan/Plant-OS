#include <libc-base.h>

#if !NO_STD

dlexport void *xmalloc(usize size) {
  void *ptr = malloc(size);
  if (ptr == null) abort();
  return ptr;
}

#endif
