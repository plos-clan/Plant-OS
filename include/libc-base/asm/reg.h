#pragma once
#include <define.h>

#define asm_get_flags()                                                                            \
  ({                                                                                               \
    size_t flags;                                                                                  \
    asm volatile("pushf\n\t"                                                                       \
                 "pop %0\n\t"                                                                      \
                 : "=r"(flags)                                                                     \
                 :);                                                                               \
    flags;                                                                                         \
  })

#define asm_set_flags(flags)                                                                       \
  ({                                                                                               \
    asm volatile("push %0\n\t"                                                                     \
                 "popf\n\t"                                                                        \
                 :                                                                                 \
                 : "r"((size_t)(flags)));                                                          \
  })

#define asm_is_sti (asm_get_flags() & (1 << 9))

#define asm_is_cli (!asm_is_sti)

#define asm_get_sp()                                                                               \
  ({                                                                                               \
    size_t sp;                                                                                     \
    asm volatile("mov %%esp, %0\n\t" : "=r"(sp));                                                  \
    sp;                                                                                            \
  })

#define asm_set_sp(sp) ({ asm volatile("mov %0, %%esp\n\t" ::"r"((size_t)(sp))); })
