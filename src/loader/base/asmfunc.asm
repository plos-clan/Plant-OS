	[BITS 32]
	EXTERN flint                 ; 在 fdc.c 中定义
	GLOBAL memtest_sub
	GLOBAL move_cursor_by_idx
	GLOBAL floppy_int
	[SECTION .text]
	
	;; definition of
	;; void floppy_int();
floppy_int:
	cli
	pushad
	mov eax, esp
	push eax
	call flint
	pop eax
	popad
	sti
	iret
	
	;; definition of
	;; u32 memtest_sub(u32 start, u32 end)
memtest_sub:
	CLI
	PUSH EDI                     ; （由于还要使用EBX, ESI, EDI）
	PUSH ESI
	PUSH EBX
	MOV ESI, 0xaa55aa55          ; pat0 = 0xaa55aa55;
	MOV EDI, 0x55aa55aa          ; pat1 = 0x55aa55aa;
	MOV EAX, [ESP + 12 + 4]      ; i = start;
	MOV dword[testsize], 1024 * 1024 * 1024 ; testsize = 1024 * 1024 * 1024;
mts_loop:
	MOV EBX, EAX
	ADD EBX, [testsize]          ; p = i + testsize;
	SUB EBX, 4                   ; p - = 4;
	MOV EDX, [EBX]               ; old = * p;
	MOV [EBX], ESI               ; * p = pat0;
	XOR DWORD [EBX], 0xffffffff  ; * p ^= 0xffffffff;
	CMP EDI, [EBX]               ; if ( * p != pat1) goto fin;
	JNE mts_fin
	XOR DWORD [EBX], 0xffffffff  ; * p ^= 0xffffffff;
	CMP ESI, [EBX]               ; if ( * p != pat0) goto fin;
	JNE mts_fin
	MOV [EBX], EDX               ; * p = old;
	ADD EAX, [testsize]          ; i + = testsize;
	CMP EAX, [ESP + 12 + 8]      ; if (i <= end) goto mts_loop;
	
	JBE mts_loop
	STI
	POP EBX
	POP ESI
	POP EDI
	RET
mts_fin:
	CMP dword[testsize], 0x1000  ; if (testsize == 0x1000) goto mts_nomore;
	JE mts_nomore
	SHR dword[testsize], 2       ; testsize / = 4;
	JMP mts_loop
mts_nomore:
	STI
	MOV [EBX], EDX               ; * p = old;
	POP EBX
	POP ESI
	POP EDI
	RET
	
move_cursor_by_idx:           ;移动光标
	ret
	
	global into_kernel
into_kernel:
	mov ebx, [esp + 4]
	mov edx, [esp + 8]
	push ebx
	push edx
	jmp far [esp]
	[SECTION .data]
testsize: dd 0
