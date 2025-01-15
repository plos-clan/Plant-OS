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

#define FLAGS_CF     MASK(0)  // 进位标志
#define FLAGS_PF     MASK(2)  // 奇偶标志
#define FLAGS_AF     MASK(4)  // 辅助进位标志
#define FLAGS_ZF     MASK(6)  // 零标志
#define FLAGS_SF     MASK(7)  // 符号标志
#define FLAGS_TF     MASK(8)  // 跟踪标志
#define FLAGS_IF     MASK(9)  // 中断允许标志
#define FLAGS_DF     MASK(10) // 方向标志
#define FLAGS_OF     MASK(11) // 溢出标志
#define FLAGS_IOPL_0 ((usize)0 << 12)
#define FLAGS_IOPL_1 ((usize)1 << 12)
#define FLAGS_IOPL_2 ((usize)2 << 12)
#define FLAGS_IOPL_3 ((usize)3 << 12)

typedef struct FLAGS {
  usize cf         : 1;
  usize reserved_1 : 1;
  usize pf         : 1;
  usize reserved_2 : 1;
  usize af         : 1;
  usize reserved_3 : 1;
  usize zf         : 1;
  usize sf         : 1;
  usize tf         : 1;
  usize if_        : 1;
  usize df         : 1;
  usize of         : 1;
  usize iopl       : 2;
  usize nt         : 1;
  usize reserved_4 : 1;
  usize rf         : 1;
  usize vm         : 1;
  usize ac         : 1;
  usize vif        : 1;
  usize vip        : 1;
  usize id         : 1;
  usize reserved_5 : 10;
} FLAGS;
