	bits 32
	extern DOSLDR_MAIN, init_gdtidt
  global _start
	
	section .text
_start:
	cli
	cld
	call init_gdtidt
	jmp 2 * 8:.next
.next:
	mov ax, 1 * 8
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov gs, ax
	mov fs, ax
	mov esp, stack_end
	jmp DOSLDR_MAIN
	
	section .bss
stack:
	resb 0x1000
stack_end:
