#include <libc-base.h>
#include <sys.h>

static unsigned int rand_seed = 1;

int rand() {
  rand_seed ^= rand_seed << 13;
  rand_seed ^= rand_seed >> 17;
  rand_seed ^= rand_seed << 5;
  return rand_seed & INT32_MAX;
}

void srand(unsigned int seed) {
  rand_seed = seed;
}

void malloc_test() {
#define M 2048
#define N (32 * 1024)
  srand(114514);
  static void  *ptr_list[M];
  static size_t size_list[M];
  static u32    hash_list[M];
  for (size_t i = 0; i < M; i++) {
    size_list[i] = rand() % 256;
    ptr_list[i]  = malloc(size_list[i]);
    for (size_t j = 0; j < size_list[i]; j++) {
      ((u8 *)ptr_list[i])[j] = rand() >> 8;
    }
    hash_list[i] = memhash(ptr_list[i], size_list[i]);
  }
  for (size_t i = 0; i < N; i++) {
    size_t n    = rand() % M;
    size_t hash = memhash(ptr_list[n], size_list[n]);
    if (hash_list[n] != hash) exit(-1);
    free(ptr_list[n]);
    size_list[n] = rand() % 256;
    ptr_list[n]  = malloc(size_list[n]);
    for (size_t j = 0; j < size_list[n]; j++) {
      ((u8 *)ptr_list[n])[j] = rand() >> 8;
    }
    hash_list[n] = memhash(ptr_list[n], size_list[n]);
  }
  for (size_t i = 0; i < M; i++) {
    size_t hash = memhash(ptr_list[i], size_list[i]);
    if (hash_list[i] != hash) exit(-1);
    free(ptr_list[i]);
  }
}

int main() {
  malloc_test();
  // char s[50];
  // sprintf(s, "%p %08x\n", __syscall(SYSCALL_GETHEAP), __syscall(SYSCALL_HEAPSZ));
  // print(s);
  // void *p = (void *)__syscall(SYSCALL_MMAP, NULL, 0x1000);
  // sprintf(p, "MMAP IN %p\n", p);
  // print(p);
  // __syscall(SYSCALL_MUNMAP, p, 0x1000);
  // sprintf(p, "MMAP IN %p\n", p);
  // print(p);
  // p = (void *)__syscall(SYSCALL_MMAP, NULL, 0x1000);
  // sprintf(p, "MMAP IN %p\n", p);
  // print(p);
  return 0;
}
