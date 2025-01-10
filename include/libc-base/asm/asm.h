#pragma once
#include <define.h>

#define ASMFUNC extern __attr(naked)

// HLT
#define asm_hlt ({ asm volatile("hlt\n\t" ::: "memory"); })

// 关闭中断
#define asm_cli ({ asm volatile("cli\n\t" ::: "memory"); })

// 启用中断
#define asm_sti ({ asm volatile("sti\n\t" ::: "memory"); })

#define asm_getreg(reg)                                                                            \
  ({                                                                                               \
    size_t _var;                                                                                   \
    asm volatile("mov %%" #reg ", %0\n\t" : "=r"(_var));                                           \
    _var;                                                                                          \
  })

#define asm_setreg(reg, value) ({ asm volatile("mov %0, %%" #reg "\n\t" ::"r"((size_t)(value))); })

#define used_val1(value)      ({ asm volatile("" ::"r,m"(value)); })
#define used_val2(value, ...) ({ used_val1(value), used_val1(__VA_ARGS__); })
#define used_val3(value, ...) ({ used_val1(value), used_val2(__VA_ARGS__); })
#define used_val4(value, ...) ({ used_val1(value), used_val3(__VA_ARGS__); })
#define used_val5(value, ...) ({ used_val1(value), used_val4(__VA_ARGS__); })
#define used_val6(value, ...) ({ used_val1(value), used_val5(__VA_ARGS__); })
#define used_val(...)         CONCAT(used_val, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

/**
 *\brief 加载 GDT
 *
 *\param addr     GDT 地址
 *\param len      GDT 条目数
 */
finline void load_gdt(void *addr, size_t len) {
  struct __PACKED__ {
    u16   limit;
    void *base;
  } gdt_r = {(u16)(len * 8 - 1), addr};
  asm volatile("lgdt %0\n\t" ::"m"(gdt_r));
}

/**
 *\brief 加载 IDT
 *
 *\param addr     IDT 地址
 *\param len      IDT 条目数
 */
finline void load_idt(void *addr, size_t len) {
  struct __PACKED__ {
    u16   limit;
    void *base;
  } idt_r = {(u16)(len * 8 - 1), addr};
  asm volatile("lidt %0\n\t" ::"m"(idt_r));
}

/**
 *\brief 加载 TSS
 *
 *\param selector 选择子
 */
finline void load_tr(size_t selector) {
  asm volatile("ltr %0\n\t" : : "m"(selector));
}

#define with_no_interrupts(...)                                                                    \
  ({                                                                                               \
    const bool _is_sti_ = asm_is_sti;                                                              \
    asm_cli;                                                                                       \
    (__VA_ARGS__);                                                                                 \
    if (_is_sti_) asm_sti;                                                                         \
  })
