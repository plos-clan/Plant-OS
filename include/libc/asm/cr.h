#pragma once
#include <type.h>

// cr0 控制寄存器
// cr1 是保留寄存器
// cr2 只读，存放最近一次发生的页错误的线性地址
// cr3 存放页目录表的基地址
// cr4 控制寄存器

#define asm_get_cr0()                                                                              \
  ({                                                                                               \
    size_t cr0;                                                                                    \
    asm volatile("mov %%cr0, %0\n\t" : "=r"(cr0));                                                 \
    cr0;                                                                                           \
  })

#define asm_set_cr0(cr0) ({ asm volatile("mov %0, %%cr0\n\t" ::"r"((size_t)(cr0))); })

#define asm_get_cr2()                                                                              \
  ({                                                                                               \
    size_t cr2;                                                                                    \
    asm volatile("mov %%cr2, %0\n\t" : "=r"(cr2));                                                 \
    cr2;                                                                                           \
  })

#define asm_get_cr3()                                                                              \
  ({                                                                                               \
    size_t cr3;                                                                                    \
    asm volatile("mov %%cr3, %0\n\t" : "=r"(cr3));                                                 \
    cr3;                                                                                           \
  })

#define asm_set_cr3(cr3) ({ asm volatile("mov %0, %%cr3\n\t" ::"r"((size_t)(cr3))); })

#define asm_get_cr4()                                                                              \
  ({                                                                                               \
    size_t cr4;                                                                                    \
    asm volatile("mov %%cr4, %0\n\t" : "=r"(cr4));                                                 \
    cr4;                                                                                           \
  })

#define asm_set_cr4(cr4) ({ asm volatile("mov %0, %%cr4\n\t" ::"r"((size_t)(cr4))); })

// set: 设置标志  clr: 清除标志
// PE: 保护模式
// MP: 协处理器
// EM: 仿真 x87
// TS: 任务切换
// ET: 扩展类型
// NE: 数学协处理器 (应该不会被使用)
// WP: 写保护
// AM: 对齐检查
// NW: 无写缓冲
// CD: 缓存禁止
// PG: 分页

#define asm_set_pe asm_set_cr0(asm_get_cr0() | CR0_PE)
#define asm_clr_pe asm_set_cr0(asm_get_cr0() & ~CR0_PE)

#define asm_set_mp asm_set_cr0(asm_get_cr0() | CR0_MP)
#define asm_clr_mp asm_set_cr0(asm_get_cr0() & ~CR0_MP)

#define asm_set_em asm_set_cr0(asm_get_cr0() | CR0_EM)
#define asm_clr_em asm_set_cr0(asm_get_cr0() & ~CR0_EM)

#define asm_set_ts asm_set_cr0(asm_get_cr0() | CR0_TS)
#define asm_clr_ts asm_set_cr0(asm_get_cr0() & ~CR0_TS)

#define asm_set_et asm_set_cr0(asm_get_cr0() | CR0_ET)
#define asm_clr_et asm_set_cr0(asm_get_cr0() & ~CR0_ET)

#define asm_set_ne asm_set_cr0(asm_get_cr0() | CR0_NE)
#define asm_clr_ne asm_set_cr0(asm_get_cr0() & ~CR0_NE)

#define asm_set_wp asm_set_cr0(asm_get_cr0() | CR0_WP)
#define asm_clr_wp asm_set_cr0(asm_get_cr0() & ~CR0_WP)

#define asm_set_am asm_set_cr0(asm_get_cr0() | CR0_AM)
#define asm_clr_am asm_set_cr0(asm_get_cr0() & ~CR0_AM)

#define asm_set_nw asm_set_cr0(asm_get_cr0() | CR0_NW)
#define asm_clr_nw asm_set_cr0(asm_get_cr0() & ~CR0_NW)

#define asm_set_cd asm_set_cr0(asm_get_cr0() | CR0_CD)
#define asm_clr_cd asm_set_cr0(asm_get_cr0() & ~CR0_CD)

#define asm_set_pg asm_set_cr0(asm_get_cr0() | CR0_PG)
#define asm_clr_pg asm_set_cr0(asm_get_cr0() & ~CR0_PG)
