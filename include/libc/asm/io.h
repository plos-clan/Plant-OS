#pragma once
#include <type.h>

// 	MOV EDX, [ESP + 4]            ; port
// 	MOV EAX, 0
// 	IN AL, DX
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
