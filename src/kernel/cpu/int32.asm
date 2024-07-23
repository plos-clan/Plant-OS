[BITS 32]
section .data

section .text
; 
; Protected Mode BIOS Call Functionailty v2.0 - by Napalm
; -------------------------------------------------------
; 
; This is code shows how its POSSIBLE to execute BIOS interrupts
; by switch out to real-mode and then back into protected mode.
; 
; If you wish to use all or part of this code you must agree
; to the license at the following URL.
; 
; License: http://creativecommons.org/licenses/by-sa/2.0/uk/
;         
; Notes: This file is in NASM syntax.
;        Turn off paging before calling these functions.
;        int32() resets all selectors.
;
; C Prototype:
;	void _cdelc int32(u8 intnum, regs16_t *regs);
; 
; Example of usage:
;   regs.ax = 0x0013;
;   int32(0x10, &regs);
;   memset((char *)0xA0000, 1, (320*200));
;   memset((char *)0xA0000 + (100*320+80), 14, 80);
;   regs.ax = 0x0000;
;   int32(0x16, &regs);
;   regs.ax = 0x0003;
;   int32(0x10, &regs);
; 
; 
;   改写（原来根本用不了）+注释翻译 By zhouzhihao 2022
;
global int32

struc regs16_t
	.di	resw 1
	.si	resw 1
	.bp	resw 1
	.sp resw 1
	.bx	resw 1
	.dx	resw 1
	.cx	resw 1
	.ax	resw 1
	.gs	resw 1
	.fs	resw 1
	.es	resw 1
	.ds	resw 1
	.ef resw 1
endstruc

%define ADR_BOTPAK 							   0
%define INT32_BASE                             0x7C00	; 这个位置分页线性地址=分段线性地址
%define REBASE(x)                              (((x) - reloc) + INT32_BASE)	; 计算在INT32_BASE中的数据的偏移地址
%define DATA32                                 1 * 8
%define CODE32                                 1000 * 8
%define CODE16                                 1001 * 8
%define DATA16                                 1002 * 8
%define STACK16                                (INT32_BASE - regs16_t_size)	; 实模式堆栈只需要存寄存器就好了 所以不用多大
int32:
		cli                     			; 禁止中断
		mov eax,0x400000
		mov cr3,eax
		; 复制reloc~int32_end的代码到INT32_BASE(0x7c00)
		mov  esi, ADR_BOTPAK + reloc
		mov  edi, INT32_BASE
		mov  ecx, (int32_end - reloc)
		cld									; 复制方向（正向
		rep  movsb
		; jmp	dword	CODE32:INT32_BASE	; 这样不好返回系统
		
		mov	eax,CODE32						; 这样调用更好返回系统
		push	eax
		mov	eax,INT32_BASE
		push	eax
		call	far	[esp]					; 相当于 call dword CODE32:INT32_BASE
		add	esp,8							; 丢弃栈中数据
		mov eax,0x400000
		mov cr3,eax
		ret
reloc:                               		; by Napalm
		mov  [REBASE(stack32_ptr)], esp        ; 保存保护模式的esp
		sidt [REBASE(idt32_ptr)]               ; 保存保护模式的IDT
		lea  esi, [esp+20]                   ; esi = 保护模式堆栈中的intnum指针
		lodsd                                  ; al = intnum
		mov  byte[REBASE(ib)], al                  ; 把INT机器码补全
		mov  esi, [esi]                        ; esi = 寄存器指针
		mov  edi, STACK16                      ; edi = 实模式堆栈地址
		mov  ecx, regs16_t_size                ; ecx = 寄存器结构体的大小
		mov  esp, edi
		rep  movsb                             ; 将保护模式下堆栈中的寄存器结构体拷贝到实模式堆栈
		
		; 因为要复制（分页线性地址和分段线性地址不同）数据 所以推后关分页
		mov  eax, cr0
		and  eax, ~0x80000000				; 关闭内存分页
		mov  cr0, eax
		
		jmp  dword CODE16:REBASE(p_mode16)      ; 跳到16位保护模式（为切换回实模式做缓冲
[bits 16]
p_mode16:
		mov  ax, DATA16							; 用于缓冲的无用选择子
		mov  ds, ax
		mov  es, ax
		mov  fs, ax
		mov  gs, ax
		mov  ss, ax
		mov  eax, cr0                          ; 关闭保护模式
		and  al, ~0x01
		mov  cr0, eax
		jmp  word 0x0000:REBASE(r_mode16)      ; 跳回实模式
r_mode16:
		xor  ax, ax
		mov  ds, ax
		mov  ss, ax
		lidt [REBASE(idt16_ptr)]               ; 切换回实模式的IDT(IVT)
		mov  bx, 0x0870
		call resetpic                          ; 设置PIC成实模式对应状态
		popa                                   ; 加载复制到堆栈的寄存器
		pop  gs
		pop  fs
		pop  es
		pop  ds
		sti                                    ; 开中断
		db 0xCD                                ; INT的机器码
	ib: db 0x00
		cli                                    ; 关中断
		xor  sp, sp
		mov  ss, sp
		mov  sp, INT32_BASE                    ; 把sp设置回来（之前pop了
		pushf                                  ; 把寄存器返回到堆栈
		push ds
		push es
		push fs
		push gs
		pusha
		mov  bx, 0x2028
		call resetpic                          ; 设置PIC成保护模式对应状态
		mov  eax, cr0                          ; 打开保护模式
		inc  eax
		mov  cr0, eax
		jmp  dword CODE32:REBASE(p_mode32)     ; 切换到32位保护模式
[bits 32]
p_mode32:
		mov  ax, DATA32                        ; 设置回原来的ds
		mov  ds, ax
		mov  es, ax
		mov  fs, ax
		mov  gs, ax
		mov  ss, ax

		; 因为要复制（分页线性地址和分段线性地址不同）数据 提前开分页
		mov  eax, cr0						; 开启内存分页
		or   eax, 0x80000000
		mov  cr0, eax
		mov eax,0x400000
		mov cr3,eax
		lidt [REBASE(idt32_ptr)]               ; 设置回原来的IDT
		mov  esp, [REBASE(stack32_ptr)]        ; 设置回原来的esp
		mov  esi, STACK16                      ; 将实模式堆栈中的寄存器复制回保护模式堆栈中的寄存器结构体（返回的数据
											   ; esi = 实模式堆栈地址
		lea  edi, [esp+24]
		mov  edi, [edi]                        ; edi = 保护模式堆栈中的寄存器结构体指针
		mov  ecx, regs16_t_size                ; ecx = 寄存器结构体的大小
		cld                                    ; 复制方向（正向
		rep  movsb
		;sti                                    ; 开中断
		retf                                    ; 返回上面的call far
		
resetpic:                                  ; reset's 8259 master and slave pic vectors
		push ax                                ; expects bh = master vector, bl = slave vector
		mov  al, 0x11                          ; 0x11 = ICW1_INIT | ICW1_ICW4
		out  0x20, al                          ; send ICW1 to master pic
		out  0xA0, al                          ; send ICW1 to slave pic
		mov  al, bh                            ; get master pic vector param
		out  0x21, al                          ; send ICW2 aka vector to master pic
		mov  al, bl                            ; get slave pic vector param
		out  0xA1, al                          ; send ICW2 aka vector to slave pic
		mov  al, 0x04                          ; 0x04 = set slave to IRQ2
		out  0x21, al                          ; send ICW3 to master pic
		shr  al, 1                             ; 0x02 = tell slave its on IRQ2 of master
		out  0xA1, al                          ; send ICW3 to slave pic
		shr  al, 1                             ; 0x01 = ICW4_8086
		out  0x21, al                          ; send ICW4 to master pic
		out  0xA1, al                          ; send ICW4 to slave pic
		pop  ax                                ; restore ax from stack
		ret                                    ; return to caller
		
stack32_ptr:                               ; address in 32bit stack after we
	dd 0x00000000                          ;   save all general purpose registers
		
idt32_ptr:                                 ; IDT table pointer for 32bit access
	dw 0x0000                              ; table limit (size)
	dd 0x00000000                          ; table base address
		
idt16_ptr:                                 ; IDT table pointer for 16bit access
	dw 0x03FF                              ; table limit (size)
	dd 0x00000000                          ; table base address

int32_end:                                 ; end marker (so we can copy the code)