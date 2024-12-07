	[BITS 32]
	section .data
	global asm_default_inthandler
	extern default_inthandler
	global asm_inthandler21, asm_inthandler20
	extern inthandler21, inthandler20, syscall, inthandler2c, signal_deal
	global asm_inthandler36, asm_inthandler2c, floppy_int, interrput_exit
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
	
asm_inthandler36:
	push ds
	push es
	push fs
	push gs
	pusha
	call syscall
	mov dword [esp + 28], eax
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
	
	extern flint
floppy_int:
	push ds
	push es
	push fs
	push gs
	pusha
	call flint
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
	
	EXTERN PCNET_IRQ
	GLOBAL PCNET_ASM_INTHANDLER
PCNET_ASM_INTHANDLER:
	push ds
	push es
	push fs
	push gs
	pusha
	;call PCNET_IRQ
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
	
	EXTERN RTL8139_IRQ
	GLOBAL RTL8139_ASM_INTHANDLER
RTL8139_ASM_INTHANDLER:
	push ds
	push es
	push fs
	push gs
	pusha
	;call RTL8139_IRQ
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
	
	EXTERN sb16_handler, hda_interrupt_handler
	GLOBAL asm_sb16_handler, asm_rtc_handler, asm_hda_handler
asm_sb16_handler:
	push ds
	push es
	push fs
	push gs
	pusha
	call sb16_handler
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
	
asm_hda_handler:
	push ds
	push es
	push fs
	push gs
	pusha
	CALL hda_interrupt_handler
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
	
	EXTERN ide_irq, rtc_irq
	GLOBAL asm_ide_irq
asm_ide_irq:
	push ds
	push es
	push fs
	push gs
	pusha
	PUSH ES
	PUSH DS
	PUSHAD
	MOV EAX, ESP
	PUSH EAX
	MOV AX, SS
	MOV DS, AX
	MOV ES, AX
	CALL ide_irq
	POP EAX
	;call signal_deal
	POPAD
	POP DS
	POP ES
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
asm_rtc_handler:
	push ds
	push es
	push fs
	push gs
	pusha
	;call rtc_irq
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
	global asm_net_api
	extern net_api
asm_net_api:
	push ds
	push es
	push fs
	push gs
	pusha
	STI
	PUSH DS
	PUSH ES
	PUSHAD                       ; 用于保存的PUSH
	PUSHAD
	MOV AX, SS
	MOV DS, AX                   ; 将操作系统用段地址存入DS和ES
	MOV ES, AX
	;CALL net_api
	ADD ESP, 32
	;call signal_deal
	POPAD
	POP ES
	POP DS
	add esp, 32
	pop gs
	pop fs
	pop es
	pop ds
	iret
asm_inthandler20:
	push ds
	push es
	push fs
	push gs
	pusha
	; 这里必须要有 因为从v86到这里，必须要切换段寄存器
	mov ax,ss
	mov ds,ax
	mov es,ax
	call inthandler20
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
asm_inthandler21:
	push ds
	push es
	push fs
	push gs
	pusha
	call inthandler21
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
asm_inthandler2c:
	push ds
	push es
	push fs
	push gs
	pusha
	call inthandler2c
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
interrput_exit:
	popa
	pop gs
	pop fs
	pop es
	pop ds
	iret
