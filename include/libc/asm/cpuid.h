#pragma once
#include <type.h>

#pragma GCC system_header

#define cpuid(id, eax, ebx, ecx, edx)                                                              \
  ({ asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(id) : "memory"); })

#define cpuid_coreid                                                                               \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    (ebx >> 24) & 0xff;                                                                            \
  })
