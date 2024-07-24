#pragma once
#include <type.h>

// PUSHFD
// POP EAX
#define asm_get_flags()                                                                            \
  ({                                                                                               \
    size_t flags;                                                                                  \
    asm volatile("pushfl\n\t"                                                                      \
                 "pop %%eax\n\t"                                                                   \
                 : "=a"(flags)                                                                     \
                 :                                                                                 \
                 : "memory");                                                                      \
    flags;                                                                                         \
  })

// POP EAX
// POPFD
#define asm_set_flags(flags)                                                                       \
  ({                                                                                               \
    size_t          __arg1         = (size_t)(flags);                                              \
    register size_t _a1 asm("eax") = __arg1;                                                       \
    asm volatile("push %%eax\n\t"                                                                  \
                 "popfl\n\t"                                                                       \
                 :                                                                                 \
                 : "r"(_a1)                                                                        \
                 : "memory");                                                                      \
    (void)0;                                                                                       \
  })
