	; v86_service.asm
	; v86服务，处理对BIOS功能的访问
	; Created by min0911 on 2024 / 10 / 17.
	; this file is under MIT license.
	
	bits 16
	; * * NOTE * * : the code should be started from here
entry:
	mov ax, 0x300
	mov ds, ax
.wait_loop:
	mov ax, [0]
	cmp ax, 2                    ; 等待请求
	jne .wait_loop
.process_request:
	mov bx, [2]
	mov cx, ds
	mov ax, cs
	mov ds, ax
	mov byte [.code.func], bl
	mov ds, cx
	mov ss, cx
	; 加载寄存器
	mov sp, 4
	popa
	pop gs
	pop fs
	pop es
	pop ds
	mov sp, 0x1000
.code:
	db 0xcd
.code.func:
	db 0x00
	mov sp, 4 + 24
	push ds
	push es
	push fs
	push gs
	pusha
	mov ax, 0x300
	mov ds, ax
	mov word [0], 3              ; 写入完成标记
	int 0x30                     ; 请求调度
	jmp .wait_loop
