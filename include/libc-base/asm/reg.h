#pragma once
#include <define.h>

#define asm_get_sp()                                                                               \
  ({                                                                                               \
    size_t sp;                                                                                     \
    asm volatile("mov %%esp, %0\n\t" : "=r"(sp));                                                  \
    sp;                                                                                            \
  })

#define asm_set_sp(sp) ({ asm volatile("mov %0, %%esp\n\t" ::"r"((size_t)(sp))); })
