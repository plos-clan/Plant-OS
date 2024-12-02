#pragma once
#include <define.h>

#define cpuid(id, eax, ebx, ecx, edx)                                                              \
  ({ asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(id)); })

#define cpuid_ncores                                                                               \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    (ebx >> 16) & 0xff;                                                                            \
  })

#define cpuid_coreid                                                                               \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    (ebx >> 24) & 0xff;                                                                            \
  })

#define cpuid_has_cflush                                                                           \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    edx & (1 << 19);                                                                               \
  })

#define cpuid_cflush_size                                                                          \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    ((ebx >> 8) & 0xff) * 8;                                                                       \
  })

// 获取是否支持 SSE

// 可参考：https://wiki.osdev.org/SSE

#define cpuid_has_sse                                                                              \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    edx & (1 << 25);                                                                               \
  })

#define cpuid_has_sse2                                                                             \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    edx & (1 << 26);                                                                               \
  })

#define cpuid_has_sse3                                                                             \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    ecx & (1 << 0);                                                                                \
  })

#define cpuid_has_ssse3                                                                            \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    ecx & (1 << 9);                                                                                \
  })

#define cpuid_has_sse41                                                                            \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    ecx & (1 << 19);                                                                               \
  })

#define cpuid_has_sse42                                                                            \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    ecx & (1 << 20);                                                                               \
  })

#define cpuid_has_sse4a                                                                            \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    ecx & (1 << 6);                                                                                \
  })

#define cpuid_has_xop                                                                              \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    ecx & (1 << 11);                                                                               \
  })

#define cpuid_has_fma4                                                                             \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    ecx & (1 << 16);                                                                               \
  })

#define cpuid_has_cvt16                                                                            \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    ecx & (1 << 29);                                                                               \
  })

#define cpuid_has_avx                                                                              \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(1, eax, ebx, ecx, edx);                                                                  \
    ecx & (1 << 28);                                                                               \
  })

#define cpuid_has_avx2                                                                             \
  ({                                                                                               \
    size_t eax, ebx, ecx, edx;                                                                     \
    cpuid(7, eax, ebx, ecx, edx);                                                                  \
    ebx & (1 << 5);                                                                                \
  })
