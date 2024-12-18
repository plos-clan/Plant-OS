#pragma once
#include <define.h>

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

#define asm_setreg(reg, val) ({ asm volatile("mov %0, %%" #reg "\n\t" : : "r"((size_t)(val))); })

#define used_val(val) ({ asm volatile("" : "r,m"(val) : : "memory"); })

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

#define with_no_interrupts(code)                                                                   \
  ({                                                                                               \
    var _flag_ = asm_get_flags();                                                                  \
    asm_cli;                                                                                       \
    (code);                                                                                        \
    asm_set_flags(_flag_);                                                                         \
  })
