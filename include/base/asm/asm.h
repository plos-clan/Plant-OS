#pragma once

// HLT
#define asm_hlt ({ asm volatile("hlt\n\t"); })

// 关闭中断
#define asm_cli ({ asm volatile("cli\n\t"); })

// 启用中断
#define asm_sti ({ asm volatile("sti\n\t"); })
