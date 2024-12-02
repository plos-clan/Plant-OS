#pragma once
#include <define.h>

#define asm_in8(port)                                                                              \
  ({                                                                                               \
    u8 _data_;                                                                                     \
    asm volatile("inb %1, %0\n\t" : "=a"(_data_) : "d"((u16)(port)) : "memory");                   \
    _data_;                                                                                        \
  })

#define asm_in16(port)                                                                             \
  ({                                                                                               \
    u16 _data_;                                                                                    \
    asm volatile("inw %1, %0\n\t" : "=a"(_data_) : "d"((u16)(port)) : "memory");                   \
    _data_;                                                                                        \
  })

#define asm_in32(port)                                                                             \
  ({                                                                                               \
    u32 _data_;                                                                                    \
    asm volatile("inl %1, %0\n\t" : "=a"(_data_) : "d"((u16)(port)) : "memory");                   \
    _data_;                                                                                        \
  })

#define asm_out8(port, data)                                                                       \
  ({ asm volatile("outb %1, %0\n\t" : : "d"((u16)(port)), "a"((u8)(data)) : "memory"); })

#define asm_out16(port, data)                                                                      \
  ({ asm volatile("outw %1, %0\n\t" : : "d"((u16)(port)), "a"((u16)(data)) : "memory"); })

#define asm_out32(port, data)                                                                      \
  ({ asm volatile("outl %1, %0\n\t" : : "d"((u16)(port)), "a"((u32)(data)) : "memory"); })
