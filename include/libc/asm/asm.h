#pragma once

// 	HLT
// 	RET
#define asm_hlt asm volatile("hlt\n\t")

// 	CLI
// 	RET
#define asm_cli asm volatile("cli\n\t")

// 	STI
// 	RET
#define asm_sti asm volatile("sti\n\t")