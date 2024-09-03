	[BITS 32]
	section .data
	GLOBAL asm_inthandler21, asm_inthandler20, asm_inthandler72
	EXTERN inthandler21, inthandler20, syscall, inthandler2c, signal_deal
	GLOBAL asm_inthandler36, asm_inthandler2c, floppy_int, interrput_exit
	section .text
	global empty_inthandler
empty_inthandler:
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
	extern custom_inthandler
asm_inthandler72:
	push ds
	push es
	push fs
	push gs
	pusha
	PUSH DS
	PUSH ES
	PUSHAD                       ; 用于保存的PUSH
	PUSHAD
	MOV AX, SS
	MOV DS, AX                   ; 将操作系统用段地址存入DS和ES
	MOV ES, AX
	;CALL custom_inthandler
	ADD ESP, 32
	call signal_deal
	POPAD
	POP ES
	POP DS
	add esp, 32
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
	PUSH ES
	PUSH DS
	PUSHAD
	MOV EAX, ESP
	PUSH EAX
	MOV AX, SS
	MOV DS, AX
	MOV ES, AX
	CALL flint
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
	
	EXTERN PCNET_IRQ
	GLOBAL PCNET_ASM_INTHANDLER
PCNET_ASM_INTHANDLER:
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
	;CALL PCNET_IRQ
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
	
	EXTERN RTL8139_IRQ
	GLOBAL RTL8139_ASM_INTHANDLER
RTL8139_ASM_INTHANDLER:
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
	;CALL RTL8139_IRQ
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
	
	EXTERN sb16_handler, hda_interrupt_handler
	GLOBAL asm_sb16_handler, asm_rtc_handler, asm_hda_handler
asm_sb16_handler:
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
	CALL sb16_handler
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
asm_hda_handler:
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
	CALL hda_interrupt_handler
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
	;CALL ide_irq
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
	PUSH ES
	PUSH DS
	PUSHAD
	MOV EAX, ESP
	add EAX, 32
	PUSH EAX
	MOV AX, SS
	MOV DS, AX
	MOV ES, AX
	MOV EAX, 0
	MOV AX, CS
	PUSH EAX
	CALL inthandler20
	pop eax
	POP EAX
	call signal_deal
	POPAD
	POP DS
	POP ES
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
	PUSH ES
	PUSH DS
	PUSHAD
	MOV EAX, ESP
	PUSH EAX
	MOV AX, SS
	MOV DS, AX
	MOV ES, AX
	CALL inthandler21
	POP EAX
	call signal_deal
	POPAD
	POP DS
	POP ES
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
	CALL inthandler2c
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
