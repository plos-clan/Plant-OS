	[BITS 32]
	section .data
	GLOBAL asm_get_flags, asm_set_flags
	GLOBAL move_cursor_by_idx
	GLOBAL memtest_sub, start_app
	GLOBAL return_to_app, entering_v86
	
	section .text
	%define ADR_BOTPAK 0x0
	
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
	
move_cursor_by_idx:           ;移动光标
	mov dx, 03d4h                ;03d4h是索引端口
	mov al, 0eh                  ;内部的0eh位置存放着光标位置的高八位
	out dx, al
	inc dx                       ;03d5h是数据端口用于读写数据
	in al, dx                    ;读取光标的高八位并且放入bh
	mov bh, al
	
	dec dx                       ;这儿开始读取光标位置的低八位放入bl
	mov al, 0fh                  ;0fh位置存放着光标位置的低八位
	out dx, al
	inc dx
	in al, dx
	mov bl, al
	
	mov word bx, [esp + 4]       ;获取参数中的光标位置
	
	mov dx, 03d4h                ;这段代码将改变后的光标位置写入端口内相应的地方以便下次访问
	mov al, 0eh                  ;写入光标位置高八位
	out dx, al
	inc dx
	mov al, bh
	out dx, al
	
	dec dx
	mov al, 0fh                  ;写入光标位置低八位
	out dx, al
	inc dx
	mov al, bl
	out dx, al
	ret
	
	extern mtask_current
	global task_switch, task_start
task_switch:
	push ebp
	mov ebp, esp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	push eax
	mov eax, [mtask_current]
	mov [eax], esp               ; 保存esp
	mov eax, [ebp + 8]           ; next
	mov [mtask_current], eax
	mov esp, [eax]
	mov eax, [eax + 4]
	mov cr3, eax
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
	ret
task_start:
	mov eax, [esp + 4]           ; next
	mov [mtask_current], eax
	mov esp, [eax]
	mov eax, [eax + 4]
	mov cr3, eax
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
	ret
	; return_to_app:
	; mov eax, 0x56
	; int 0x36
	; popa
	; pop gs
	; pop fs
	; pop es
	; pop ds
	; ret
	; retuen_to_app_end:
	
	; extern void entering_v86(u32 ss, u32 esp, u32 cs, u32 eip);
entering_v86:
	mov ebp, esp                 ; save stack pointer
	push dword [ebp + 4]         ; ss
	push dword [ebp + 8]         ; esp
	pushfd                       ; eflags
	or dword [esp], (1 << 17)    ; set VM flags
	push dword [ebp + 12]        ; cs
	push dword [ebp + 16]        ; eip
	iret
	
	[SECTION .data]
testsize: dd 0
