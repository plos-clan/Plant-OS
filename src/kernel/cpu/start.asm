;
; start.asm : the beginning of Plant-OS kernel
; 
; MIT LICENSE
;
; Copyright (C) 2024~ plos-clan
;
magic   equ 0xe85250d6
i386    equ 0
length  equ header_end - header_start

section .multiboot2
header_start:
    dd magic  ; 魔数
    dd i386   ; 32位保护模式
    dd length ; 头部长度
    dd -(magic + i386 + length); 校验和

    ; 结束标记
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:
section .ram_disk
global ram_disk_addr,ram_disk_end
ram_disk_addr: dd _raw_ram_disk_addr
ram_disk_end: dd _raw_ram_disk_end
_raw_ram_disk_addr:
; 先编译一遍 然后改成你的hd.img
;incbin "/home/min0911/Plant-OS/bin/hd.img"
_raw_ram_disk_end:



global _start
extern kernel_main,init_gdtidt
section .text
_start:
    xchg bx,bx
    cli
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
