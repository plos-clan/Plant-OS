#pragma once

// HLT
#define asm_hlt ({ asm volatile("hlt\n\t"); })

// 关闭中断
#define asm_cli ({ asm volatile("cli\n\t"); })

// 启用中断
#define asm_sti ({ asm volatile("sti\n\t"); })

#define asm_load(reg)                                                                              \
  ({                                                                                               \
    size_t _var;                                                                                   \
    asm volatile("mov %%" #reg ", %0\n\t" : "=r"(_var));                                           \
    _var;                                                                                          \
  })

#define asm_save(reg, val) ({ asm volatile("mov %0, %%" #reg "\n\t" : : "r"((size_t)(val))); })
