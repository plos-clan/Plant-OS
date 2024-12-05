
#if 0
#  include <kernel.h>

#  define klogd(...) ((void)0)

void malloc_test() {
  size_t old_count = system_tick;
  klogw("%d", old_count);

#  define M 2048
#  define N (32 * 1024 * 1024)
  srand(114514);
  static void  *ptr_list[M];
  static size_t size_list[M];
  static u32    hash_list[M];
  klogi("alloc...");
  for (size_t i = 0; i < M; i++) {
    size_list[i] = rand() % 256;
    ptr_list[i]  = malloc(size_list[i]);
    // for (size_t j = 0; j < size_list[i]; j++) {
    //   ((u8 *)ptr_list[i])[j] = rand() >> 8;
    // }
    // hash_list[i] = memhash(ptr_list[i], size_list[i]);
    klogd("step 1: %d alloced: %p", i, ptr_list[i]);
  }
  klogi("test...");
  for (size_t i = 0; i < N; i++) {
    size_t n = rand() % M;
    // size_t hash = memhash(ptr_list[n], size_list[n]);
    // if (hash_list[n] != hash) {
    //   info("step 2: %d:%d check: %p", i, n, ptr_list[n]);
    //   fatal("hash error: %u != %u", hash, hash_list[n]);
    // }
    free(ptr_list[n]);
    klogd("step 2: %d:%d freed: %p", i, n, ptr_list[n]);
    size_list[n] = rand() % 256;
    ptr_list[n]  = malloc(size_list[n]);
    // for (size_t j = 0; j < size_list[n]; j++) {
    //   ((u8 *)ptr_list[n])[j] = rand() >> 8;
    // }
    // hash_list[n] = memhash(ptr_list[n], size_list[n]);
    klogd("step 2: %d:%d alloced: %p", i, n, ptr_list[n]);
    if (i % (N / 100) == 0) klogi("  %d%%", i / (N / 100));
  }
  klogi("free...");
  for (size_t i = 0; i < M; i++) {
    // size_t hash = memhash(ptr_list[i], size_list[i]);
    // if (hash_list[i] != hash) {
    //   klogd("step 3: %d check: %p", i, ptr_list[i]);
    //   fatal("hash error: %u != %u", hash, hash_list[i]);
    // }
    free(ptr_list[i]);
    klogd("step 3: %d freed: %p", i, ptr_list[i]);
  }

  size_t new_count = system_tick;
  klogw("%d", new_count);
  klogw("%d", new_count - old_count);

  fatal("test end.");
}
#endif
