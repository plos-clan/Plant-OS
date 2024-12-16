	[BITS 32]
	section .data
	GLOBAL asm_error0, asm_error1, asm_error3, asm_error4, asm_error5
	GLOBAL asm_error6, asm_error7, asm_error8, asm_error9, asm_error10
	GLOBAL asm_error11, asm_error12, asm_error13, asm_error14, asm_error16
	GLOBAL asm_error17, asm_error18
	section .text
	EXTERN ERROR0, ERROR1, ERROR3, ERROR4, ERROR5, ERROR6, ERROR7, ERROR8
	EXTERN ERROR9, ERROR10, ERROR11, ERROR12, ERROR13, ERROR16
	EXTERN ERROR17, ERROR18
	EXTERN KILLAPP
asm_error0:
	cli
	mov ecx, 0
	mov edx, dword [esp + 4]
	mov ax, fs
	cmp ax, 1 * 8                ; 是不是在程序产生的
	jne KILLAPP1
	call ERROR0
	iret
asm_error1:
	cli
	mov ecx, 1
	mov edx, dword [esp + 4]
	mov ax, fs
	cmp ax, 1 * 8                ; 是不是在程序产生的
	jne KILLAPP1
	call ERROR1
	iret
asm_error3:
	cli
	mov ecx, 3
	mov edx, dword [esp + 4]
	mov ax, fs
	cmp ax, 1 * 8                ; 是不是在程序产生的
	jne KILLAPP1
	call ERROR3
	iret
asm_error4:
	cli
	mov ecx, 4
	mov edx, dword [esp + 4]
	mov ax, fs
	cmp ax, 1 * 8                ; 是不是在程序产生的
	jne KILLAPP1
	call ERROR4
	iret
asm_error5:
	cli
	mov ecx, 5
	mov edx, dword [esp + 4]
	mov ax, fs
	cmp ax, 1 * 8                ; 是不是在程序产生的
	jne KILLAPP1
	call ERROR5
	iret
asm_error6:
	cli
	mov ecx, 6
	mov edx, dword [esp]
	mov ax, fs
	cmp ax, 1 * 8                ; 是不是在程序产生的
	jne KILLAPP1
	call ERROR6
	iret
asm_error7:
	push ds
	push es
	push fs
	push gs
	pusha
	call ERROR7
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
asm_error8:
	cli
	mov ecx, 8
	mov edx, dword [esp + 4]
	mov ax, fs
	cmp ax, 1 * 8                ; 是不是在程序产生的
	jne KILLAPP1
	call ERROR8
	iret
asm_error9:
	iret
asm_error10:
	cli
	push 10
	mov ax, fs
	cmp ax, 1 * 8                ; 是不是在程序产生的
	jne KILLAPP1
	call ERROR10
	iret
asm_error11:
	cli
	mov ecx, 11
	mov edx, dword [esp + 4]
	mov ax, fs
	cmp ax, 1 * 8                ; 是不是在程序产生的
	jne KILLAPP1
	call ERROR11
	iret
asm_error12:
	cli
	mov ecx, 12
	mov edx, dword [esp + 4]
	mov ax, fs
	cmp ax, 1 * 8                ; 是不是在程序产生的
	jne KILLAPP1
	call ERROR12
	iret
asm_error13:
	push ds
	push es
	push fs
	push gs
	pusha
	mov ax, ss
	mov ds, ax
	mov es, ax
	mov gs, ax
	mov fs, ax
	call ERROR13
	popa
	pop gs
	pop fs
	pop es
	pop ds
	add esp, 4
	xchg bx, bx
	iret
asm_error16:
	cli
	mov ecx, 16
	mov edx, dword [esp + 4]
	mov ax, fs
	cmp ax, 1 * 8                ; 是不是在程序产生的
	jne KILLAPP1
	call ERROR16
	iret
asm_error17:
	cli
	mov ecx, 17
	mov edx, dword [esp + 4]
	mov ax, fs
	cmp ax, 1 * 8                ; 是不是在程序产生的
	jne KILLAPP1
	call ERROR17
	iret
asm_error18:
	cli
	mov ecx, 18
	mov edx, dword [esp + 4]
	mov ax, fs
	cmp ax, 1 * 8                ; 是不是在程序产生的
	jne KILLAPP1
	call ERROR18
	iret
KILLAPP1:
	cli
	PUSHAD
	mov ax, ss                   ; 切fs gs
	mov ds, ax
	mov es, ax
	POPAD
	push ecx
	push edx
	sti
	call KILLAPP
	add esp, 12
	jmp $
