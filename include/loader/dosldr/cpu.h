#pragma once
#include <define.h>
#include <type.h>
// 	HLT
#define asm_hlt asm volatile("hlt\n\t")

// 	CLI
#define asm_cli asm volatile("cli\n\t")

// 	STI
#define asm_sti asm volatile("sti\n\t")
#define asm_in8(port)                                                                              \
  ({                                                                                               \
    ssize_t          data;                                                                         \
    ssize_t          __arg1         = (ssize_t)(port);                                             \
    register ssize_t _a1 asm("edx") = __arg1;                                                      \
    asm volatile("inb %%dx, %%al\n\t" : "=a"(data) : "r"(_a1) : "memory");                         \
    data;                                                                                          \
  })

// 	MOV EDX, [ESP + 4]            ; port
// 	MOV EAX, 0
// 	IN AX, DX
#define asm_in16(port)                                                                             \
  ({                                                                                               \
    ssize_t          data;                                                                         \
    ssize_t          __arg1         = (ssize_t)(port);                                             \
    register ssize_t _a1 asm("edx") = __arg1;                                                      \
    asm volatile("inw %%dx, %%ax\n\t" : "=a"(data) : "r"(_a1) : "memory");                         \
    data;                                                                                          \
  })

// 	MOV EDX, [ESP + 4]            ; port
// 	IN EAX, DX
#define asm_in32(port)                                                                             \
  ({                                                                                               \
    ssize_t          data;                                                                         \
    ssize_t          __arg1         = (ssize_t)(port);                                             \
    register ssize_t _a1 asm("edx") = __arg1;                                                      \
    asm volatile("inl %%dx, %%eax\n\t" : "=a"(data) : "r"(_a1) : "memory");                        \
    data;                                                                                          \
  })

// 	MOV EDX, [ESP + 4]            ; port
// 	MOV AL, [ESP + 8]             ; data
// 	OUT DX, AL
#define asm_out8(port, data)                                                                       \
  ({                                                                                               \
    ssize_t          __arg1         = (ssize_t)(data);                                             \
    ssize_t          __arg2         = (ssize_t)(port);                                             \
    register ssize_t _a2 asm("edx") = __arg2;                                                      \
    register ssize_t _a1 asm("eax") = __arg1;                                                      \
    asm volatile("outb %%al, %%dx\n\t" : : "r"(_a1), "r"(_a2) : "memory");                         \
    (void)0;                                                                                       \
  })

// 	MOV EDX, [ESP + 4]            ; port
// 	MOV EAX, [ESP + 8]            ; data
// 	OUT DX, AX
#define asm_out16(port, data)                                                                      \
  ({                                                                                               \
    ssize_t          __arg1         = (ssize_t)(data);                                             \
    ssize_t          __arg2         = (ssize_t)(port);                                             \
    register ssize_t _a2 asm("edx") = __arg2;                                                      \
    register ssize_t _a1 asm("eax") = __arg1;                                                      \
    asm volatile("outw %%ax, %%dx\n\t" : : "r"(_a1), "r"(_a2) : "memory");                         \
    (void)0;                                                                                       \
  })

// 	MOV EDX, [ESP + 4]            ; port
// 	MOV EAX, [ESP + 8]            ; data
// 	OUT DX, EAX
#define asm_out32(port, data)                                                                      \
  ({                                                                                               \
    ssize_t          __arg1         = (ssize_t)(data);                                             \
    ssize_t          __arg2         = (ssize_t)(port);                                             \
    register ssize_t _a2 asm("edx") = __arg2;                                                      \
    register ssize_t _a1 asm("eax") = __arg1;                                                      \
    asm volatile("outl %%eax, %%dx\n\t" : : "r"(_a1), "r"(_a2) : "memory");                        \
    (void)0;                                                                                       \
  })

int  io_load_eflags();
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
int  load_cr0(void);
void store_cr0(int cr0);

#define ADR_IDT      0x0026f800
#define LIMIT_IDT    0x000007ff
#define ADR_GDT      0x00270000
#define LIMIT_GDT    0x0000ffff
#define ADR_BOTPAK   0x100000
#define LIMIT_BOTPAK 0x0007ffff
#define AR_DATA32_RW 0x4092
#define AR_DATA16_RW 0x0092
#define AR_CODE32_ER 0x409a
#define AR_CODE16_ER 0x009a
#define AR_INTGATE32 0x008e

struct SEGMENT_DESCRIPTOR {
  i16  limit_low, base_low;
  char base_mid, access_right;
  char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
  i16  offset_low, selector;
  char dw_count, access_right;
  i16  offset_high;
};

void init_gdtidt(void);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);