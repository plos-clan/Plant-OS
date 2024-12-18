#pragma once
#include <define.h>

#define asm_rdmsr(msr, lo, hi) asm volatile("rdmsr\n\t" : "=a"(lo), "=d"(hi) : "c"(msr)::"memory")
#define asm_wrmsr(msr, lo, hi) asm volatile("wrmsr\n\t" ::"a"(lo), "d"(hi), "c"(msr) : "memory")

//+注意 IA32_SYSENTER_CS 的约定：
//   Ring 0 的代码段 CS0
//   Ring 0 的数据段 DS0 = CS0 + 8
//   Ring 3 的代码段 CS3 = CS0 + 16
//   Ring 3 的数据段 DS3 = CS0 + 24
//   如果有 REX.W 前缀则：
//     CS3 = CS0 + 32
//     DS3 = CS0 + 40
#define IA32_SYSENTER_CS  0x174
#define IA32_SYSENTER_ESP 0x175
#define IA32_SYSENTER_EIP 0x176
