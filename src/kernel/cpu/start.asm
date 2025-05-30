	;
	; start.asm : the beginning of Plant - OS kernel
	;
	; MIT LICENSE
	;
	; Copyright (C) 2024~ plos - clan
	;
	[bits 32]
	magic equ 0xe85250d6
	i386 equ 0
	length equ header_end - header_start
	
	%define RING0_CS 1 * 8
	%define RING0_DS 2 * 8
	
	section .multiboot2
header_start:
	dd magic                     ; 魔数
	dd i386                      ; 32位保护模式
	dd length                    ; 头部长度
	dd - (magic + i386 + length) ; 校验和
	
	; 结束标记
	dw 0                         ; type
	dw 0                         ; flags
	dd 8                         ; size
header_end:
	
	global _start
	extern kernel_main, init_gdtidt, init_error_inthandler
	section .text
_start:
	cli
	call init_gdtidt
	call init_error_inthandler
jmp RING0_CS:.next
.next:
	mov ax, RING0_DS
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov gs, ax
	mov fs, ax
	mov esp, stack_end
	jmp kernel_main
	jmp header_start
	
	section .bss
stack:
	resb 4096
stack_end:
