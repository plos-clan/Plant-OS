;
; start.asm : the beginning of Plant-OS kernel
; 
; MIT LICENSE
;
; Copyright (C) 2024~ plos-clan
;

global _start
extern kernel_main,init_gdtidt
section .text
_start:
    cli
    mov esp,0xfffff
    call init_gdtidt
    jmp 2*8:next
next:
    mov ax,1*8
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov gs,ax
    mov fs,ax
    mov esp,stack_end
    jmp kernel_main
section .bss
stack: resb 0x1000
stack_end:
