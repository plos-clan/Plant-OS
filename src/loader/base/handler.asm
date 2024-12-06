	bits 32
	global asm_default_inthandler
	extern default_inthandler
	section .text
	
asm_default_inthandler:
	push ds
	push es
	push fs
	push gs
	pusha
	call default_inthandler
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
