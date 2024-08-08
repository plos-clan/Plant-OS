#pragma once
#include <define.h>

#define asm_in8(port)                                                                              \
  ({                                                                                               \
    size_t          data;                                                                          \
    size_t          __arg1         = (size_t)(port);                                               \
    register size_t _a1 asm("edx") = __arg1;                                                       \
    asm volatile("inb %%dx, %%al\n\t" : "=a"(data) : "r"(_a1) : "memory");                         \
    data;                                                                                          \
  })

#define asm_in16(port)                                                                             \
  ({                                                                                               \
    size_t          data;                                                                          \
    size_t          __arg1         = (size_t)(port);                                               \
    register size_t _a1 asm("edx") = __arg1;                                                       \
    asm volatile("inw %%dx, %%ax\n\t" : "=a"(data) : "r"(_a1) : "memory");                         \
    data;                                                                                          \
  })

#define asm_in32(port)                                                                             \
  ({                                                                                               \
    size_t          data;                                                                          \
    size_t          __arg1         = (size_t)(port);                                               \
    register size_t _a1 asm("edx") = __arg1;                                                       \
    asm volatile("inl %%dx, %%eax\n\t" : "=a"(data) : "r"(_a1) : "memory");                        \
    data;                                                                                          \
  })

#define asm_out8(port, data)                                                                       \
  ({                                                                                               \
    size_t          __arg1         = (size_t)(data);                                               \
    size_t          __arg2         = (size_t)(port);                                               \
    register size_t _a2 asm("edx") = __arg2;                                                       \
    register size_t _a1 asm("eax") = __arg1;                                                       \
    asm volatile("outb %%al, %%dx\n\t" : : "r"(_a1), "r"(_a2) : "memory");                         \
    (void)0;                                                                                       \
  })

#define asm_out16(port, data)                                                                      \
  ({                                                                                               \
    size_t          __arg1         = (size_t)(data);                                               \
    size_t          __arg2         = (size_t)(port);                                               \
    register size_t _a2 asm("edx") = __arg2;                                                       \
    register size_t _a1 asm("eax") = __arg1;                                                       \
    asm volatile("outw %%ax, %%dx\n\t" : : "r"(_a1), "r"(_a2) : "memory");                         \
    (void)0;                                                                                       \
  })

#define asm_out32(port, data)                                                                      \
  ({                                                                                               \
    size_t          __arg1         = (size_t)(data);                                               \
    size_t          __arg2         = (size_t)(port);                                               \
    register size_t _a2 asm("edx") = __arg2;                                                       \
    register size_t _a1 asm("eax") = __arg1;                                                       \
    asm volatile("outl %%eax, %%dx\n\t" : : "r"(_a1), "r"(_a2) : "memory");                        \
    (void)0;                                                                                       \
  })
