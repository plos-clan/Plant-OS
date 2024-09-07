#pragma once

#define asm_getu(addr)                                                         \
  ({                                                                           \
    size_t _var;                                                               \
    asm volatile("mov (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));       \
    _var;                                                                      \
  })

#define asm_getu8(addr)                                                        \
  ({                                                                           \
    uint8_t _var;                                                              \
    asm volatile("movb (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));      \
    _var;                                                                      \
  })

#define asm_getu16(addr)                                                       \
  ({                                                                           \
    uint16_t _var;                                                             \
    asm volatile("movw (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));      \
    _var;                                                                      \
  })

#define asm_getu32(addr)                                                       \
  ({                                                                           \
    uint32_t _var;                                                             \
    asm volatile("movl (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));      \
    _var;                                                                      \
  })

#define asm_getu64(addr)                                                       \
  ({                                                                           \
    uint64_t _var;                                                             \
    asm volatile("movq (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));      \
    _var;                                                                      \
  })

#define asm_geti(addr)                                                         \
  ({                                                                           \
    ssize_t _var;                                                              \
    asm volatile("mov (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));       \
    _var;                                                                      \
  })

#define asm_geti8(addr)                                                        \
  ({                                                                           \
    int8_t _var;                                                               \
    asm volatile("movb (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));      \
    _var;                                                                      \
  })

#define asm_geti16(addr)                                                       \
  ({                                                                           \
    int16_t _var;                                                              \
    asm volatile("movw (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));      \
    _var;                                                                      \
  })

#define asm_geti32(addr)                                                       \
  ({                                                                           \
    int32_t _var;                                                              \
    asm volatile("movl (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));      \
    _var;                                                                      \
  })

#define asm_geti64(addr)                                                       \
  ({                                                                           \
    int64_t _var;                                                              \
    asm volatile("movq (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));      \
    _var;                                                                      \
  })

#define asm_get(addr) asm_getu(addr)
#define asm_get8(addr) asm_getu8(addr)
#define asm_get16(addr) asm_getu16(addr)
#define asm_get32(addr) asm_getu32(addr)

#define asm_set(addr, val)                                                     \
  ({                                                                           \
    asm volatile("mov %0, (%1)\n\t"                                            \
                 :                                                             \
                 : "r"((size_t)(val)), "r"((void *)(addr)));                   \
  })

#define asm_set8(addr, val)                                                    \
  ({                                                                           \
    asm volatile("movb %0, (%1)\n\t"                                           \
                 :                                                             \
                 : "r"((uint8_t)(val)), "r"((void *)(addr)));                  \
  })

#define asm_set16(addr, val)                                                   \
  ({                                                                           \
    asm volatile("movw %0, (%1)\n\t"                                           \
                 :                                                             \
                 : "r"((uint16_t)(val)), "r"((void *)(addr)));                 \
  })

#define asm_set32(addr, val)                                                   \
  ({                                                                           \
    asm volatile("movl %0, (%1)\n\t"                                           \
                 :                                                             \
                 : "r"((uint32_t)(val)), "r"((void *)(addr)));                 \
  })

#define asm_set64(addr, val)                                                   \
  ({                                                                           \
    asm volatile("movq %0, (%1)\n\t"                                           \
                 :                                                             \
                 : "r"((uint64_t)(val)), "r"((void *)(addr)));                 \
  })
