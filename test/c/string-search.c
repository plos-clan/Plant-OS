#include <stdio.h>
#include <stdlib.h>

#define NO_STD 0
#include <algorithm.h>

int main() {
  assert(kmp_search("hello, world!", "world") == 7);
  assert(kmp_search("hello, world!", "hello") == 0);
  assert(kmp_search("hello, world!", "lo") == 3);
  assert(kmp_search("hello, world!", "wl") == -1);
  assert(bm_search("hello, world!", "world") == 7);
  assert(bm_search("hello, world!", "hello") == 0);
  assert(bm_search("hello, world!", "lo") == 3);
  assert(bm_search("hello, world!", "wl") == -1);
  return 0;
}
