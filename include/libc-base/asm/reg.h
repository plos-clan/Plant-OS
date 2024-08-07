#pragma once
#include <type.h>

#define asm_get_flags()                                                                            \
  ({                                                                                               \
    size_t flags;                                                                                  \
    asm volatile("pushfl\n\t"                                                                      \
                 "pop %0\n\t"                                                                      \
                 : "=r"(flags)                                                                     \
                 :);                                                                               \
    flags;                                                                                         \
  })

#define asm_set_flags(flags)                                                                       \
  ({                                                                                               \
    asm volatile("push %0\n\t"                                                                     \
                 "popfl\n\t"                                                                       \
                 :                                                                                 \
                 : "r"((size_t)(flags)));                                                          \
    (void)0;                                                                                       \
  })

#define asm_get_sp()                                                                               \
  ({                                                                                               \
    size_t sp;                                                                                     \
    asm volatile("mov %%esp, %0\n\t" : "=r"(sp));                                                  \
    sp;                                                                                            \
  })

#define asm_set_sp(sp) ({ asm volatile("mov %0, %%esp\n\t" ::"r"((size_t)(sp))); })

#define asm_get_cr0()                                                                              \
  ({                                                                                               \
    size_t cr0;                                                                                    \
    asm volatile("mov %%cr0, %0\n\t" : "=r"(cr0));                                                 \
    cr0;                                                                                           \
  })

#define asm_set_cr0(cr0) ({ asm volatile("mov %0, %%cr0\n\t" ::"r"((size_t)(cr0))); })
