#pragma once
#include <define.h>
#include <type.h>

typedef bool spin_t;

#define spin_trylock(_spin_) (!atom_tas(&(_spin_)))
#define spin_lock(_spin_)                                                                          \
  ({                                                                                               \
    while (atom_tas(&(_spin_))) {}                                                                 \
  })
#define spin_unlock(_spin_) atom_clr(&(_spin_))

//

finline void *memcpy(void *_rest _d, const void *_rest _s, size_t _n);
finline void *memset(void *_s, int _c, size_t _n);
finline void  bzero(void *_s, size_t _n);
finline void  explicit_bzero(void *_s, size_t _n);

//

finline char  *strcpy(char *_rest d, cstr _rest s);
finline char  *strncpy(char *_rest d, cstr _rest s, size_t n);
finline int    strcmp(cstr _s1, cstr _s2);
finline int    strncmp(cstr _s1, cstr _s2, size_t n);
finline char  *strdup(cstr _s);
finline char  *strndup(cstr _s, size_t _n);
finline size_t strlen(cstr _s);
finline size_t strnlen(cstr _s, size_t _l);

#define streq(s1, s2) (((s1) && (s2)) ? strcmp(s1, s2) == 0 : (s1) == (s2))

// 基本内存分配函数

dlimport void *malloc(size_t size) __attr_malloc;
dlimport void  free(void *ptr);
dlimport void *calloc(size_t n, size_t size);
dlimport void *realloc(void *ptr, size_t newsize);

// 基本输出函数

dlimport int printf(cstr _rest fmt, ...);
