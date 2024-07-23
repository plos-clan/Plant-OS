#pragma once

// 	MOV EDX, [ESP + 4]            ; port
// 	MOV EAX, 0
// 	IN AL, DX
// 	RET
#define asm_in8(port)                                                                              \
  ({                                                                                               \
    __INTPTR_TYPE__          data;                                                                 \
    __INTPTR_TYPE__          __arg1         = (__INTPTR_TYPE__)(port);                             \
    register __INTPTR_TYPE__ _a1 asm("edx") = __arg1;                                              \
    asm volatile("inb %%dx, %%al\n\t" : "=a"(data) : "r"(_a1) : "memory");                         \
    data;                                                                                          \
  })

// 	MOV EDX, [ESP + 4]            ; port
// 	MOV EAX, 0
// 	IN AX, DX
// 	RET
#define asm_in16(port)                                                                             \
  ({                                                                                               \
    __INTPTR_TYPE__          data;                                                                 \
    __INTPTR_TYPE__          __arg1         = (__INTPTR_TYPE__)(port);                             \
    register __INTPTR_TYPE__ _a1 asm("edx") = __arg1;                                              \
    asm volatile("inw %%dx, %%ax\n\t" : "=a"(data) : "r"(_a1) : "memory");                         \
    data;                                                                                          \
  })

// 	MOV EDX, [ESP + 4]            ; port
// 	IN EAX, DX
// 	RET
#define asm_in32(port)                                                                             \
  ({                                                                                               \
    __INTPTR_TYPE__          data;                                                                 \
    __INTPTR_TYPE__          __arg1         = (__INTPTR_TYPE__)(port);                             \
    register __INTPTR_TYPE__ _a1 asm("edx") = __arg1;                                              \
    asm volatile("inl %%dx, %%eax\n\t" : "=a"(data) : "r"(_a1) : "memory");                        \
    data;                                                                                          \
  })

// 	MOV EDX, [ESP + 4]            ; port
// 	MOV AL, [ESP + 8]             ; data
// 	OUT DX, AL
// 	RET
#define asm_out8(port, data)                                                                       \
  ({                                                                                               \
    __INTPTR_TYPE__          __arg1         = (__INTPTR_TYPE__)(data);                             \
    __INTPTR_TYPE__          __arg2         = (__INTPTR_TYPE__)(port);                             \
    register __INTPTR_TYPE__ _a2 asm("edx") = __arg2;                                              \
    register __INTPTR_TYPE__ _a1 asm("eax") = __arg1;                                              \
    asm volatile("outb %%al, %%dx\n\t" : : "r"(_a1), "r"(_a2) : "memory");                         \
    (void)0;                                                                                       \
  })

// 	MOV EDX, [ESP + 4]            ; port
// 	MOV EAX, [ESP + 8]            ; data
// 	OUT DX, AX
// 	RET
#define asm_out16(port, data)                                                                      \
  ({                                                                                               \
    __INTPTR_TYPE__          __arg1         = (__INTPTR_TYPE__)(data);                             \
    __INTPTR_TYPE__          __arg2         = (__INTPTR_TYPE__)(port);                             \
    register __INTPTR_TYPE__ _a2 asm("edx") = __arg2;                                              \
    register __INTPTR_TYPE__ _a1 asm("eax") = __arg1;                                              \
    asm volatile("outw %%ax, %%dx\n\t" : : "r"(_a1), "r"(_a2) : "memory");                         \
    (void)0;                                                                                       \
  })

// 	MOV EDX, [ESP + 4]            ; port
// 	MOV EAX, [ESP + 8]            ; data
// 	OUT DX, EAX
// 	RET
#define asm_out32(port, data)                                                                      \
  ({                                                                                               \
    __INTPTR_TYPE__          __arg1         = (__INTPTR_TYPE__)(data);                             \
    __INTPTR_TYPE__          __arg2         = (__INTPTR_TYPE__)(port);                             \
    register __INTPTR_TYPE__ _a2 asm("edx") = __arg2;                                              \
    register __INTPTR_TYPE__ _a1 asm("eax") = __arg1;                                              \
    asm volatile("outl %%eax, %%dx\n\t" : : "r"(_a1), "r"(_a2) : "memory");                        \
    (void)0;                                                                                       \
  })
