#pragma once
#include <type.h>

// PUSHFD
// POP EAX
#define asm_get_flags()                                                                            \
  ({                                                                                               \
    size_t flags;                                                                                  \
    asm volatile("pushfl\n\t"                                                                      \
                 "pop %0\n\t"                                                                      \
                 : "=r"(flags)                                                                     \
                 :                                                                                 \
                 : "memory");                                                                      \
    flags;                                                                                         \
  })

// POP EAX
// POPFD
#define asm_set_flags(flags)                                                                       \
  ({                                                                                               \
    asm volatile("push %0\n\t"                                                                     \
                 "popfl\n\t"                                                                       \
                 :                                                                                 \
                 : "r"((size_t)(flags))                                                            \
                 : "memory");                                                                      \
    (void)0;                                                                                       \
  })
