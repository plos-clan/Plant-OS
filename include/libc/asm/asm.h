#pragma once

#define asm_hlt asm volatile("hlt\n\t")
#define asm_cli asm volatile("cli\n\t")
#define asm_sti asm volatile("sti\n\t")

// asm_hlt:                       ; void asm_hlt(void);
// 	HLT
// 	RET
//
// asm_cli:                       ; void asm_cli(void);
// 	CLI
// 	RET
//
// asm_sti:                       ; void asm_sti(void);
// 	STI
// 	RET
