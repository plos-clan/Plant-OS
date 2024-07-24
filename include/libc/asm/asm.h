#pragma once

// 	HLT
#define asm_hlt asm volatile("hlt\n\t" ::: "memory")

// 	CLI
#define asm_cli asm volatile("cli\n\t" ::: "memory")

// 	STI
#define asm_sti asm volatile("sti\n\t" ::: "memory")