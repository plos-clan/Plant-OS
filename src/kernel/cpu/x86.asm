	[bits 32]
	global memtest_sub
	
	section .text
	
memtest_sub:                  ; u32 memtest_sub(u32 start, u32 end)
	CLI
	PUSH EDI                     ; （由于还要使用EBX, ESI, EDI）
	PUSH ESI
	PUSH EBX
	MOV ESI, 0xaa55aa55          ; pat0 = 0xaa55aa55;
	MOV EDI, 0x55aa55aa          ; pat1 = 0x55aa55aa;
	MOV EAX, [ESP + 12 + 4]      ; i = start;
	MOV dword[testsize], 1024 * 1024 * 1024 ; testsize = 1024 * 1024 * 1024;
.mts_loop:
	pushad
	push eax
	add esp, 4
	popad
	MOV EBX, EAX
	ADD EBX, [testsize]          ; p = i + testsize;
	SUB EBX, 4                   ; p - = 4;
	MOV EDX, [EBX]               ; old = * p;
	MOV [EBX], ESI               ; * p = pat0;
	XOR DWORD [EBX], 0xffffffff  ; * p ^= 0xffffffff;
	CMP EDI, [EBX]               ; if ( * p != pat1) goto fin;
	JNE .mts_fin
	XOR DWORD [EBX], 0xffffffff  ; * p ^= 0xffffffff;
	CMP ESI, [EBX]               ; if ( * p != pat0) goto fin;
	JNE .mts_fin
	MOV [EBX], EDX               ; * p = old;
	ADD EAX, [testsize]          ; i + = testsize;
	CMP EAX, [ESP + 12 + 8]      ; if (i <= end) goto mts_loop;
	
	JBE .mts_loop
	STI
	POP EBX
	POP ESI
	POP EDI
	RET
.mts_fin:
	CMP dword[testsize], 0x1000  ; if (testsize == 0x1000) goto mts_nomore;
	JE .mts_nomore
	SHR dword[testsize], 2       ; testsize / = 4;
	JMP .mts_loop
.mts_nomore:
	STI
	MOV [EBX], EDX               ; * p = old;
	POP EBX
	POP ESI
	POP EDI
	RET
	
	extern current_task
	global asm_task_switch, asm_task_start
	; 注意进入函数时必须 cli
asm_task_switch:              ; void asm_task_switch(task_t current, task_t next) FASTCALL;
	pusha
	mov [ecx], esp               ; 保存esp
asm_task_start:               ; void asm_task_start(task_t current, task_t next) FASTCALL;
	mov esp, [edx]
	mov eax, [edx + 4]
	mov cr3, eax
	popa
	sti                          ; 这边必须 sti
	ret                          ; 注意 sti 指令的实现，sti ret 会在返回之后才设置 IF 位
	
	global entering_v86
entering_v86:                 ; extern void entering_v86(u32 ss, u32 esp, u32 cs, u32 eip);
	mov ebp, esp                 ; save stack pointer
	push dword [ebp + 4]         ; ss
	push dword [ebp + 8]         ; esp
	pushfd                       ; eflags
	or dword [esp], (1 << 17)    ; set VM flags
	push dword [ebp + 12]        ; cs
	push dword [ebp + 16]        ; eip
	iret
	
	section .data
testsize: dd 0
