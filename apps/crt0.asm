	[bits 32]
	global _start
	extern __libc_start_main
	section .text
_start:
	push edx
	push esi
	push edi
	call __libc_start_main
	mov ebx, 0xffff
	mov eax, 0
	int 0x36
	ud2
	int 3
	jmp $
