#pragma once
#include <type.h>

#define asm_get_cr0()                                                                              \
  ({                                                                                               \
    size_t cr0;                                                                                    \
    asm volatile("mov %%cr0, %0\n\t" : "=r"(cr0)::"memory");                                       \
    cr0;                                                                                           \
  })

#define asm_set_cr0(cr0) ({ asm volatile("mov %0, %%cr0\n\t" ::"r"((size_t)(cr0)) : "memory"); })
