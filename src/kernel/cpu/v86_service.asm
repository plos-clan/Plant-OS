; v86_service.asm
; v86服务，处理对BIOS功能的访问
; Created by min0911 on 2024/10/17.
; this file is under MIT license.

bits 16
entry: ; **NOTE**: the code should be started from here
	mov ax,cs
	mov es,ax
	; xchg bx, bx
    mov ax,0x300
    mov ds,ax
    mov ax,0x100
    mov ss,ax
    mov sp,0xfff

.3:
    mov ax,[0]
    cmp ax,1
    jne .3
    mov word[0],2 ; busy
    xchg bx, bx
    mov bx,[2]
    push ds
    mov ax,cs
    mov ds,ax
    mov byte [.code.func],bl
    pop ds
    mov ax,0x300
    mov ss,ax
    mov bp,sp
    mov sp,0x4
    xchg bx, bx

    popa
    pop gs
	pop fs
	pop es
	pop ds
    mov sp,0xfff
    sti
.code:
    db 0xcd
.code.func:
    db 0x00
    xchg bx, bx
    mov sp,0x4+24
    push ds
    push es
    push fs
    push gs
    pusha
    mov ax,0x300
    mov ds,ax
    mov word [0],3 ; done
    int 0x30
    jmp entry
	jmp $
printstr:
	mov ah, 0x0e
	mov al,[es:si]
	cmp al,0
	je .1
	int 0x10
	inc si
	jmp printstr
.1:
	ret
msg: db "The message is shown in V86 mode by int 10h",0