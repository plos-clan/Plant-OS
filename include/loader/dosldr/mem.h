#pragma once
#include <define.h>
#include <type.h>

#define MEMMAN_FREES 4090

#define MEMMAN_ADDR 0x0005c0000
struct FREEINFO {
  u32 addr, size;
};
typedef struct MEMMAN {
  int             frees, maxfrees, lostsize, losts;
  struct FREEINFO free[MEMMAN_FREES];
} MEMMAN;

u32   memtest(u32 start, u32 end);
void  memman_init(MEMMAN *man);
u32   memman_total(MEMMAN *man);
u32   memman_alloc(MEMMAN *man, u32 size);
int   memman_free(MEMMAN *man, u32 addr, u32 size);
u32   memman_alloc_4k(MEMMAN *man, u32 size);
int   memman_free_4k(MEMMAN *man, u32 addr, u32 size);
void *page_malloc(int size);
void  page_free(void *p, int size);

void *memcpy(void *s, const void *ct, size_t n);
void *malloc(int size);
void  free(void *p);
void *realloc(void *ptr, u32 size);

void  clean(char *s, int len);
u32   memtest_sub(u32 start, u32 end);
void *memset(void *s, int c, size_t n);
int   memcmp(const void *s1, const void *s2, size_t n);
