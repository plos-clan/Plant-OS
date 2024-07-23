[BITS 32]
section .data
GLOBAL	asm_error0,asm_error1,asm_error3,asm_error4,asm_error5
GLOBAL	asm_error6,asm_error7,asm_error8,asm_error9,asm_error10
GLOBAL	asm_error11,asm_error12,asm_error13,asm_error14,asm_error16
GLOBAL	asm_error17,asm_error18
section .text
EXTERN	ERROR0,ERROR1,ERROR3,ERROR4,ERROR5,ERROR6,ERROR7,ERROR8
EXTERN	ERROR9,ERROR10,ERROR11,ERROR12,ERROR13,PF,ERROR16
EXTERN	ERROR17,ERROR18
EXTERN	KILLAPP
asm_error0:
	cli
	mov ecx,0
	mov edx, dword [esp+4]
	mov	ax,fs
	cmp	ax,1*8	; 是不是在程序产生的
	jne	KILLAPP1
	call	ERROR0
	iretd
asm_error1:
	cli
	mov ecx,1
	mov edx, dword [esp+4]
	mov	ax,fs
	cmp	ax,1*8	; 是不是在程序产生的
	jne	KILLAPP1
	call	ERROR1
	iretd
asm_error3:
	cli
	mov ecx,3
	mov edx, dword [esp+4]
	mov	ax,fs
	cmp	ax,1*8	; 是不是在程序产生的
	jne	KILLAPP1
	call	ERROR3
	iretd
asm_error4:
	cli
	mov ecx,4
	mov edx, dword [esp+4]
	mov	ax,fs
	cmp	ax,1*8	; 是不是在程序产生的
	jne	KILLAPP1
	call	ERROR4
	iretd
asm_error5:
	cli
	mov ecx,5
	mov edx, dword [esp+4]
	mov	ax,fs
	cmp	ax,1*8	; 是不是在程序产生的
	jne	KILLAPP1
	call	ERROR5
	iretd	
asm_error6:
	cli
	mov ecx,6
	mov edx, dword [esp]
	mov	ax,fs
	cmp	ax,1*8	; 是不是在程序产生的
	jne	KILLAPP1
	call	ERROR6
	iretd
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
	iretd
asm_error8:
	cli
	mov ecx,8
	mov edx, dword [esp+4]
	mov	ax,fs
	cmp	ax,1*8	; 是不是在程序产生的
	jne	KILLAPP1
	call	ERROR8
	iretd
asm_error9:
	iretd
asm_error10:
	cli
	push	10
	mov	ax,fs
	cmp	ax,1*8	; 是不是在程序产生的
	jne	KILLAPP1
	call	ERROR10
	iretd
asm_error11:
	cli
	mov ecx,11
	mov edx, dword [esp+4]
	mov	ax,fs
	cmp	ax,1*8	; 是不是在程序产生的
	jne	KILLAPP1
	call	ERROR11
	iretd
asm_error12:
	cli
	mov ecx,12
	mov edx, dword [esp+4]
	mov	ax,fs
	cmp	ax,1*8	; 是不是在程序产生的
	jne	KILLAPP1
	call	ERROR12
	iretd
asm_error13:
	cli
	PUSHAD
	mov	ax,ss	; 切fs gs
	mov	ds,ax
	mov	es,ax
	POPAD
	mov esi,0xb8000
	mov byte [esi],'G'
	inc esi
	mov byte [esi],0x0c
	inc esi
	mov byte [esi],'P'
	inc esi
	mov byte [esi],0x0c
	jmp $
	iretd
EXTERN Print_Hex
asm_error14:
	cli
  push ds
  push es
  push fs
  push gs
  pusha
	call PF
	popa
	pop gs
	pop fs
	pop es
	pop ds
	add esp,4
	 ; what the fuck is esp doing?
	sti
	iretd
asm_error16:
	cli
	mov ecx,16
	mov edx, dword [esp+4]
	mov	ax,fs
	cmp	ax,1*8	; 是不是在程序产生的
	jne	KILLAPP1
	call ERROR16
	iretd
asm_error17:
	cli
	mov ecx,17
	mov edx, dword [esp+4]
	mov	ax,fs
	cmp	ax,1*8	; 是不是在程序产生的
	jne	KILLAPP1
	call	ERROR17
	iretd
asm_error18:
	cli
	mov ecx,18
	mov edx, dword [esp+4]
	mov	ax,fs
	cmp	ax,1*8	; 是不是在程序产生的
	jne	KILLAPP1
	call	ERROR18
	iretd
KILLAPP1:
	cli
	PUSHAD
	mov	ax,ss	; 切fs gs
	mov	ds,ax
	mov	es,ax
	POPAD
	push ecx
	push edx
	sti
	call	KILLAPP
	add	esp,12
	jmp	$