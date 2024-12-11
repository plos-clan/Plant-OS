#pragma once
#include <define.h>

//; CPUID 指令
//+ 可参考：https://en.wikipedia.org/wiki/CPUID

#define cpuid(id, eax, ebx, ecx, edx)                                                              \
  ({ asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(id)); })

#define cpuid_get(id)                                                                              \
  u32 eax, ebx, ecx, edx;                                                                          \
  cpuid(id, eax, ebx, ecx, edx)

#define cpuid_core_freq                                                                            \
  ({                                                                                               \
    cpuid_get(0x15);                                                                               \
    ecx;                                                                                           \
  })

#define cpuid_tsc_freq                                                                             \
  ({                                                                                               \
    cpuid_get(0x15);                                                                               \
    (u32)((u64)ecx * ebx / eax);                                                                   \
  })

#define cpuid_bus_freq                                                                             \
  ({                                                                                               \
    cpuid_get(0x16);                                                                               \
    (ecx & 0xffff) * 1000000;                                                                      \
  })

#define cpuid_core_max_freq                                                                        \
  ({                                                                                               \
    cpuid_get(0x16);                                                                               \
    (ebx & 0xffff) * 1000000;                                                                      \
  })

#define cpuid_core_min_freq                                                                        \
  ({                                                                                               \
    cpuid_get(0x16);                                                                               \
    (eax & 0xffff) * 1000000;                                                                      \
  })

// --------------------------------------------------

#define cpuid_support_64bit                                                                        \
  ({                                                                                               \
    cpuid_get(0x80000001);                                                                         \
    edx & (1 << 29);                                                                               \
  })

// --------------------------------------------------
//; 多核

#define cpuid_ncores                                                                               \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    (ebx >> 16) & 0xff;                                                                            \
  })

#define cpuid_coreid                                                                               \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    (ebx >> 24) & 0xff;                                                                            \
  })

// --------------------------------------------------
//; 缓存

#define cpuid_has_cflush                                                                           \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    edx & (1 << 19);                                                                               \
  })

#define cpuid_cflush_size                                                                          \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    ((ebx >> 8) & 0xff) * 8;                                                                       \
  })

// --------------------------------------------------
//; 获取是否支持 SIMD 指令集
//+ 可参考：https://wiki.osdev.org/SSE

#define cpuid_has_sse                                                                              \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    edx & (1 << 25);                                                                               \
  })

#define cpuid_has_sse2                                                                             \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    edx & (1 << 26);                                                                               \
  })

#define cpuid_has_sse3                                                                             \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    ecx & (1 << 0);                                                                                \
  })

#define cpuid_has_ssse3                                                                            \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    ecx & (1 << 9);                                                                                \
  })

#define cpuid_has_sse41                                                                            \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    ecx & (1 << 19);                                                                               \
  })

#define cpuid_has_sse42                                                                            \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    ecx & (1 << 20);                                                                               \
  })

#define cpuid_has_sse4a                                                                            \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    ecx & (1 << 6);                                                                                \
  })

#define cpuid_has_xop                                                                              \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    ecx & (1 << 11);                                                                               \
  })

#define cpuid_has_fma4                                                                             \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    ecx & (1 << 16);                                                                               \
  })

#define cpuid_has_cvt16                                                                            \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    ecx & (1 << 29);                                                                               \
  })

#define cpuid_has_avx                                                                              \
  ({                                                                                               \
    cpuid_get(1);                                                                                  \
    ecx & (1 << 28);                                                                               \
  })

#define cpuid_has_avx2                                                                             \
  ({                                                                                               \
    cpuid_get(7);                                                                                  \
    ebx & (1 << 5);                                                                                \
  })
